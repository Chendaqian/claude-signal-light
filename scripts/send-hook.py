"""
Claude Code LED 状态桥接脚本（Python 版）
通过蓝牙 BLE 连接 ESP32，发送 LED 状态命令

灯效模式（0-19）：
  0  = bothoff      全灭
  1  = bothflash    三灯同频闪烁
  2  = greenflash   绿灯闪烁
  3  = redflash     红灯闪烁
  4  = yellowflash  黄灯闪烁
  5  = greenon      绿灯常亮
  6  = redon        红灯常亮
  7  = yellowon     黄灯常亮
  8  = bothon       三灯常亮
  9  = police       警车交替快闪
  10 = heartbeat    心跳双闪
  11 = sos          SOS 摩尔斯求救码
  12 = breathing    三色轮转呼吸灯
  13 = firefly      萤火虫混沌呼吸
  14 = ecg          心电波模拟
  15 = phasechase   三相正弦追逐
  16 = strobechase  急救爆闪追击
  17 = taichi       太极呼吸
  18 = hellomorse   HELLO 摩尔斯码
  19 = radar        雷达扫描与锁定

Claude Code 别名（兼容旧接口）：
  off → 0, thinking → 2, busy → 7, success → 5, error → 3, alarm → 6

用法：
  send-hook.py <mode> [buzzer_param]
    mode:         模式编号 0-19 或名称
    buzzer_param: 蜂鸣器参数
      0 = 不响
      1 = 短响一声（80ms，主板自检音）
      2 = 短响两声（两次不同声调）

  示例：
    send-hook.py 6 1        → 模式6（红灯常亮）+ 短响一声
    send-hook.py 6 2        → 模式6（红灯常亮）+ 短响两声
    send-hook.py alarm 1    → 红灯常亮 + 短响一声
    send-hook.py alarm 2    → 红灯常亮 + 短响两声
    send-hook.py alarm 0    → 红灯常亮 + 不响
    send-hook.py alarm      → 红灯常亮 + 不响
    send-hook.py 2          → 模式2（绿灯闪烁），蜂鸣器不响
"""

import asyncio
import json
import os
import subprocess
import sys
from bleak import BleakClient, BleakScanner

# ===== 配置 =====
DEVICE_NAME = "Claude-LED-LUCKEY"
#SERVICE_UUID = "1460d279-efe7-4066-a466-e80dd498c317"
#CHARACTERISTIC_UUID = "a9680f41-1111-4b02-b787-48592ff38d3b"

SERVICE_UUID = "f4b7e3a1-5c6d-4e8f-9a2b-1c3d5e7f9a0b"
CHARACTERISTIC_UUID = "8c9d4e2f-1a3b-4c5d-6e7f-8a9b0c1d2e3f"

TIMEOUT_SECONDS = 3  # 连接超时
SCAN_TIMEOUT = 3  # 扫描超时
CACHE_FILE = os.path.join(os.environ.get("TEMP", "/tmp"), DEVICE_NAME, "ble-cache.json")

# ===== PowerShell send-hook.ps1 兼容映射（-LedMode → ESP32 命令）=====
# Claude Code 状态码 → ESP32 别名
# 注意：数字输入直接发送数字，不经过此映射
CLAUDE_CODE_MAP = {
    1: "off",       # 全灭
    2: "thinking",  # 绿灯闪烁
    3: "error",     # 红灯闪烁
    4: "alarm",     # 红灯常亮
    5: "success",   # 绿灯常亮
    6: "busy",      # 黄灯常亮
    17: "17",       # 太极呼吸（SessionStart/SessionEnd）
}


def _load_cache():
    """从缓存文件读取设备地址"""
    try:
        if os.path.exists(CACHE_FILE):
            with open(CACHE_FILE, "r") as f:
                data = json.load(f)
                return data.get("address")
    except Exception:
        pass
    return None


def _save_cache(address):
    """保存设备地址到缓存文件"""
    try:
        cache_dir = os.path.dirname(CACHE_FILE)
        os.makedirs(cache_dir, exist_ok=True)
        with open(CACHE_FILE, "w") as f:
            json.dump({"address": address, "name": DEVICE_NAME}, f)
    except Exception:
        pass


async def send_command(command: str, max_retries: int = 3):
    """发送命令到 ESP32，支持重试，优先使用缓存地址"""
    import logging
    logger = logging.getLogger(__name__)

    # 先尝试用缓存地址连接
    cached_address = _load_cache()
    if cached_address:
        logger.info(f"Using cached address: {cached_address}")
        print(f"Using cached address: {cached_address}")
        try:
            async with BleakClient(cached_address, timeout=TIMEOUT_SECONDS) as client:
                if client.is_connected:
                    await client.write_gatt_char(CHARACTERISTIC_UUID, command.encode())
                    logger.info(f"Sent via cache: {command}")
                    print(f"Sent via cache: {command}")
                    return True
        except Exception as e:
            logger.warning(f"Cached address failed: {e}, falling back to scan")
            print(f"Cached address failed, scanning...")
            # 缓存失败，删除缓存
            try:
                os.remove(CACHE_FILE)
            except Exception:
                pass

    # 缓存不存在或失败，扫描设备
    for attempt in range(max_retries):
        logger.info(f"Scanning for {DEVICE_NAME} (attempt {attempt + 1}/{max_retries})...")
        print(f"Scanning for {DEVICE_NAME} (attempt {attempt + 1}/{max_retries})...")
        device = await BleakScanner.find_device_by_name(DEVICE_NAME, timeout=SCAN_TIMEOUT)

        if device is None:
            logger.warning(f"Device not found on attempt {attempt + 1}")
            print(f"Device not found on attempt {attempt + 1}")
            if attempt < max_retries - 1:
                continue
            return False

        logger.info(f"Found: {device.name} ({device.address})")
        print(f"Found: {device.name} ({device.address})")

        try:
            async with BleakClient(device, timeout=TIMEOUT_SECONDS) as client:
                logger.info(f"Connected: {client.is_connected}")
                print(f"Connected: {client.is_connected}")
                await client.write_gatt_char(CHARACTERISTIC_UUID, command.encode())
                logger.info(f"Sent: {command}")
                print(f"Sent: {command}")
                # 成功后保存地址到缓存
                _save_cache(device.address)
                return True
        except Exception as e:
            logger.error(f"Connection failed on attempt {attempt + 1}: {e}")
            print(f"Connection failed on attempt {attempt + 1}: {e}")
            if attempt < max_retries - 1:
                continue
            return False

    return False


def do_ble_send(args):
    """实际 BLE 发送逻辑"""
    import logging
    import sys
    import os
    from datetime import datetime

    # 设置日志（存放到临时目录，以设备名作为文件夹）
    log_dir = os.path.join(os.environ.get("TEMP", "/tmp"), DEVICE_NAME)
    os.makedirs(log_dir, exist_ok=True)
    log_file = os.path.join(log_dir, "python-hook.log")
    logging.basicConfig(
        filename=log_file,
        level=logging.INFO,
        format='%(asctime)s - %(message)s'
    )

    logging.info(f"=== Hook called ===")
    logging.info(f"Args: {args}")
    logging.info(f"Python executable: {sys.executable}")
    logging.info(f"Python version: {sys.version}")
    logging.info(f"Current directory: {os.getcwd()}")
    logging.info(f"PATH: {os.environ.get('PATH', 'NOT SET')}")

    arg = args[0]
    buzz_param = args[1] if len(args) > 1 else "0"

    # 判断是数字还是名称
    if arg.isdigit():
        mode_num = int(arg)
        if 0 <= mode_num <= 19:
            command = str(mode_num)
        else:
            print(f"Invalid mode: {mode_num} (valid: 0-19)")
            logging.error(f"Invalid mode: {mode_num}")
            sys.exit(1)
    else:
        command = arg.lower()

    # 拼接蜂鸣器参数
    if buzz_param.isdigit():
        buzz_val = int(buzz_param)
        if buzz_val > 0:
            command = f"{command},{buzz_param}"

    try:
        logging.info(f"Sending command: {command}")
        result = asyncio.run(send_command(command, max_retries=3))
        if not result:
            print("BLE send failed after all retries - device not found or connection failed")
            logging.error("BLE send failed after all retries - device not found or connection failed")
            sys.exit(1)
        else:
            logging.info("BLE send successful")
    except Exception as e:
        print(f"Error: {e}")
        logging.error(f"Error: {e}", exc_info=True)
        sys.exit(1)


def main():
    if len(sys.argv) < 2:
        print("Usage: send-hook.py <mode> [buzzer_param]")
        print("  mode:         0-19 or name")
        print("  buzzer_param: 0=off, 1=beep, 2=beepbeep")
        sys.exit(1)

    # 直接执行 BLE 发送（hook 有 timeout，不会阻塞太久）
    do_ble_send(sys.argv[1:])


if __name__ == "__main__":
    main()
