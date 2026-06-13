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
  send-hook.py <mode> [buzzer_duration_ms]
    mode:                模式编号 0-19 或名称
    buzzer_duration_ms:  蜂鸣器响的毫秒数，0=不响，默认0

  示例：
    send-hook.py 6 200     → 模式6（红灯常亮）+ 蜂鸣器响200ms
    send-hook.py alarm 20  → 红灯常亮 + 极短滴一声
    send-hook.py alarm 0   → 红灯常亮 + 不响
    send-hook.py alarm     → 红灯常亮 + 不响
    send-hook.py 2         → 模式2（绿灯闪烁），蜂鸣器不响
"""

import asyncio
import sys
from bleak import BleakClient, BleakScanner

# ===== 配置 =====
DEVICE_NAME = "Claude-LED-LUCKEY"
SERVICE_UUID = "12345678-1234-1234-1234-123456789abc"
CHARACTERISTIC_UUID = "12345678-1234-1234-1234-123456789abd"
TIMEOUT_SECONDS = 5

# ===== PowerShell send-hook.ps1 兼容映射（-LedMode → ESP32 命令）=====
CLAUDE_CODE_MAP = {
    1: "off",       # 全灭
    2: "thinking",  # 绿灯闪烁
    3: "busy",      # 黄灯常亮
    4: "alarm",     # 红灯常亮
    5: "success",   # 绿灯常亮
    6: "error",     # 红灯闪烁
    17: "17",       # 太极呼吸（SessionStart/SessionEnd）
}


async def send_command(command: str):
    """发送命令到 ESP32"""
    print(f"Scanning for {DEVICE_NAME}...")
    device = await BleakScanner.find_device_by_name(DEVICE_NAME, timeout=TIMEOUT_SECONDS)

    if device is None:
        print("Device not found!")
        return False

    print(f"Found: {device.name} ({device.address})")

    async with BleakClient(device, timeout=TIMEOUT_SECONDS) as client:
        print(f"Connected: {client.is_connected}")
        await client.write_gatt_char(CHARACTERISTIC_UUID, command.encode())
        print(f"Sent: {command}")

    return True


def main():
    if len(sys.argv) < 2:
        print("Usage: send-hook.py <mode> [buzzer_duration_ms]")
        print("  mode:                0-19 or name")
        print("  buzzer_duration_ms:  buzzer duration in ms, 0=off (default 0)")
        sys.exit(1)

    arg = sys.argv[1]

    # 第二个参数：蜂鸣器时长（毫秒），默认 0
    buzz_ms = sys.argv[2] if len(sys.argv) > 2 else "0"

    # 判断是数字还是名称
    if arg.isdigit():
        mode_num = int(arg)
        if mode_num in CLAUDE_CODE_MAP:
            command = CLAUDE_CODE_MAP[mode_num]
        elif 0 <= mode_num <= 19:
            command = str(mode_num)
        else:
            print(f"Invalid mode: {mode_num} (valid: 0-19)")
            sys.exit(1)
    else:
        command = arg.lower()

    # 拼接蜂鸣器参数（>0 才发送）
    if buzz_ms.isdigit() and int(buzz_ms) > 0:
        command = f"{command},{buzz_ms}"

    try:
        success = asyncio.run(send_command(command))
        sys.exit(0 if success else 1)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
