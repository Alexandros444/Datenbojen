import qrcode
from PIL import Image
import numpy as np

# Step 1: Get URL input
url = input("Enter URL: ")

# Step 2: Generate QR code
qr = qrcode.QRCode(border=1)
qr.add_data(url)
qr.make(fit=True)
img = qr.make_image(fill_color="black", back_color="white").convert('1')

# Step 3: Display QR code
img.show()
img.save(f"{url}.png")

# Step 4: Convert to C bitmap array
bitmap = np.array(img)
height, width = bitmap.shape

def to_c_bitmap(bitmap):
    c_array = []
    for row in bitmap:
        byte = 0
        bits = []
        for i, pixel in enumerate(row):
            bit = 0 if pixel else 1  # 0: white, 1: black
            bits.append(str(bit))
            byte = (byte << 1) | bit
            if (i + 1) % 8 == 0 or i == len(row) - 1:
                c_array.append(byte)
                byte = 0
    return c_array

c_bitmap = to_c_bitmap(bitmap)

# Print C bitmap code
print(f"// Bitmap size: {width}x{height}")
print("const uint8_t qr_bitmap[] = {")
for i, byte in enumerate(c_bitmap):
    print(f"  0x{byte:02X},", end='')
    if (i + 1) % (width // 8) == 0:
        print()
print("};")