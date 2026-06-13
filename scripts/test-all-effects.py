"""
自动测试全部 20 种 LED 灯效
每种灯效持续 3 秒，然后切换到下一种
"""

import asyncio
import sys
from bleak import BleakClient, BleakScanner

DEVICE_NAME = "Claude-LED-LUCKEY"
CHARACTERISTIC_UUID = "12345678-1234-1234-1234-123456789abd"
HOLD_SECONDS = 3

NAMES = [
    "0  - 全灭",
    "1  - 三灯同频闪烁 500ms",
    "2  - 绿灯闪烁 500ms",
    "3  - 红灯闪烁 500ms",
    "4  - 黄灯闪烁 500ms",
    "5  - 绿灯常亮",
    "6  - 红灯常亮",
    "7  - 黄灯常亮",
    "8  - 三灯常亮",
    "9  - 警车交替快闪",
    "10 - 心跳双闪",
    "11 - SOS 摩尔斯求救码",
    "12 - 三色轮转呼吸灯",
    "13 - 萤火虫混沌呼吸",
    "14 - 心电波模拟",
    "15 - 三相正弦追逐",
    "16 - 急救爆闪追击",
    "17 - 太极呼吸",
    "18 - HELLO 摩尔斯码",
    "19 - 雷达扫描与锁定",
]


async def main():
    print(f"Scanning for {DEVICE_NAME}...")
    device = await BleakScanner.find_device_by_name(DEVICE_NAME, timeout=5)

    if device is None:
        print("Device not found!")
        sys.exit(1)

    print(f"Found: {device.name} ({device.address})")
    print(f"Each effect holds {HOLD_SECONDS}s, total ~{20 * HOLD_SECONDS}s")
    print("=" * 40)

    async with BleakClient(device, timeout=5) as client:
        print(f"Connected: {client.is_connected}\n")

        for i in range(20):
            await client.write_gatt_char(CHARACTERISTIC_UUID, str(i).encode())
            print(f"[{i:2d}/19] {NAMES[i]}")
            await asyncio.sleep(HOLD_SECONDS)

        # 测试完关闭
        await client.write_gatt_char(CHARACTERISTIC_UUID, b"0")
        print("\n" + "=" * 40)
        print("All effects tested! LED off.")


if __name__ == "__main__":
    asyncio.run(main())
