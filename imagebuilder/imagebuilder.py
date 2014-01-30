import sys
import json

SECTOR_SIZE = 4096

#if len(sys.argv) < 2:
#    print("Usage: imagebuilder.py [image directory]", file=sys.stderr)

folderpath = "image" #sys.argv[1]

indexfile = open(folderpath + "/index.json", "r")
index = json.load(indexfile)
indexfile.close()

print("DIR:")

image = []
fileaddress = SECTOR_SIZE

# build the FAT:
for file in index:
    print(file["name"], file["nsectors"], file["file"])
    image.append(fileaddress >> 16)
    image.append((fileaddress >> 8) & 0xff)
    image.append(fileaddress & 0xff)
    for i in range(12):
        if i < len(file["name"]):
            image.append(ord(file["name"][i]))
        else:
            image.append(0)
    image.append(file["nsectors"])
    fileaddress += file["nsectors"] * SECTOR_SIZE
while len(image) < SECTOR_SIZE:
    image.append(0xff)

for file in index:
    ifile = open(folderpath + "/" + file["file"], "rb")
    nbyteswritten = 0
    padlength = file["nsectors"] * SECTOR_SIZE
    byte = ifile.read(1)
    while byte:
        image.append(int.from_bytes(byte, byteorder="big"))
        byte = ifile.read(1)
        nbyteswritten += 1
    if nbyteswritten > padlength:
        print("File " + file["file"] + " is too big (" + str(nbyteswritten) + " bytes, expected no more than" + str(padlength) +")", file=sys.stderr)
    while nbyteswritten < padlength:
        image.append(0)
        nbyteswritten += 1

ofile = open(folderpath + ".bin", "wb")
ofile.write(bytearray(image))
ofile.close()
