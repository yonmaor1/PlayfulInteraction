from bleak import BleakClient, BleakScanner
import asyncio

SERVICE_UUID = "12345678-1234-1234-1234-123456789abc"
CHARACTERISTIC_UUID = "abcd1234-abcd-1234-abcd-123456789abc"

async def main():
    print("Scanning for BLE devices...")
    devices = await BleakScanner.discover()
    for device in devices:
        print(device)

    target_device = input("Enter the name of the target device: ")
    device_address = None

    for device in devices:
        if device.name == target_device:
            device_address = device.address
            break

    if not device_address:
        print("Device not found!")
        return

    async with BleakClient(device_address) as client:
        print(f"Connected to {target_device}")
        while True:
            data = await client.read_gatt_char(CHARACTERISTIC_UUID)
            print(f"Received data: {data.decode()}")
            await asyncio.sleep(1)

if __name__ == "__main__":
    asyncio.run(main())
