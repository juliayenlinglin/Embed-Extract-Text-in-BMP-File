/* Julia Lin
 * jylin@scu.edu
 */
#include <string.h>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include "bmp.h"

namespace csenbmp {
// copy constructor
BMP::BMP(const BMP &src) { this->operator=(src); }

// copy assignment
BMP &BMP::operator=(const BMP &src) {
    bmpHdr = src.bmpHdr;
    info = src.info;
    const size_t imgSize = src.info.getImgSize();
    // deep copy
    pixels = new(std::nothrow) pixel_t[imgSize];
    if (pixels)
        memcpy((void *)pixels, (void *)src.pixels, imgSize);
    return *this;
}

// read the pixel from a specific location
Pixel24 BMP::getPixel(int x, int y) const {
    Pixel24 data;
    size_t rowBytes = info.getRowSize();
    pixel_t *p{nullptr};
    if (pixels) {
        p = pixels + rowBytes * y;
        p += x * sizeof(Pixel24);
    }
    data.b = p[0];
    data.g = p[1];
    data.r = p[2];
    return data;
}

// over-write the location with a new pixel value
void BMP::setPixel(int x, int y, Pixel24 pixel) {
    size_t rowBytes = info.getRowSize();
    if (pixels) {
        pixel_t *p = pixels + rowBytes * y + x * sizeof(Pixel24);;
        p[0] = pixel.b;
        p[1] = pixel.g;
        p[2] = pixel.r;
    }
}

// read from a file
void BMP::loadBMP(const std::string &fn) {
    std::ifstream ifs;
    ifs.open(fn, std::ifstream::in | std::ifstream::binary);
    if (ifs.fail())
        throw std::runtime_error(std::string("BMP file not opened"));
    bmpHdr.load(*this, ifs);
    info.loadInfo(*this, ifs);
    loadPixels(ifs);
    ifs.close();
}
void BMP::BMPHeader::load(const BMP &parent, std::ifstream &ifs) {
    unsigned char raw[BMP::BMPHDRSIZE];
    ifs.seekg(0, ifs.beg);
    ifs.read((char *)raw, BMP::BMPHDRSIZE);
    signature[0] = raw[0];
    signature[1] = raw[1];
    fsize = parent.fromBytes(raw+2, sizeof(fsize));
    reserve1 = parent.fromBytes(raw+6, sizeof(reserve1));
    reserve2 = parent.fromBytes(raw+8, sizeof(reserve2));
    pixelStart = parent.fromBytes(raw+10, sizeof(pixelStart));
    if (!acceptSignature())
        throw std::runtime_error(std::string("Unsupported BMP Format"));
}

void BMP::InfoHeader::loadInfo(const BMP &p, std::ifstream &ifs) {
    unsigned char raw[BMP::BMPINFOSIZE];
    ifs.read((char *)raw, BMP::BMPINFOSIZE);
    int idx = 0;
    hdrSize = p.fromBytes(raw+idx, sizeof(hdrSize)); idx += sizeof(hdrSize);
    width = p.fromBytes(raw+idx, sizeof(width)); idx += sizeof(width);
    height = p.fromBytes(raw+idx, sizeof(height)); idx += sizeof(height);
    nPlanes = p.fromBytes(raw+idx, sizeof(nPlanes)); idx += sizeof(nPlanes);
    pixelDepth = p.fromBytes(raw+idx, sizeof(pixelDepth)); idx += sizeof(pixelDepth);
    compressor = p.fromBytes(raw+idx, sizeof(compressor)); idx += sizeof(compressor);
    rawSize = p.fromBytes(raw+idx, sizeof(rawSize)); idx += sizeof(rawSize);
    horizRes = p.fromBytes(raw+idx, sizeof(horizRes)); idx += sizeof(horizRes);
    vertRes = p.fromBytes(raw+idx, sizeof(vertRes)); idx += sizeof(vertRes);
    cmapSize = p.fromBytes(raw+idx, sizeof(cmapSize)); idx += sizeof(cmapSize);
    keyColors = p.fromBytes(raw+idx, sizeof(keyColors)); idx += sizeof(keyColors);
    if (!acceptInfo()) {
        std::cerr << "HDR Size: " << hdrSize <<
            ", Pixel Depth: " << pixelDepth <<
            ", Compressor: " << compressor <<
            ", CmapSize: " << cmapSize << std::endl;
        throw std::runtime_error(std::string("Cannot handle BMP options"));
    }
}

// compute the size
// allocate buffer
// read the whole thing in
void BMP::loadPixels(std::ifstream &ifs) {
    const size_t imgSize = info.getImgSize();
    ifs.seekg(bmpHdr.getOffset(), ifs.beg);
    pixels = new (std::nothrow) pixel_t [imgSize];
    if (pixels)
        ifs.read((char *)pixels, imgSize);
}

// save the BMP Header
// then the rest
void BMP::saveBMP(const std::string &fn) const {
    std::ofstream ofs;
    ofs.open(fn, std::ifstream::out | std::ifstream::binary);
    if (ofs.fail())
        throw std::runtime_error(std::string("Cannot create file"));
    bmpHdr.save(*this, ofs);
    info.saveInfo(*this, ofs);
    savePixels(ofs);
    ofs.close();
}

void BMP::BMPHeader::save(const BMP &p, std::ofstream &ofs) const {
    unsigned char raw[BMP::BMPHDRSIZE];
    raw[0] = signature[0];
    raw[1] = signature[1];
    int idx = 2;
    
    p.toBytes(fsize,raw+idx,sizeof(fsize)); idx += sizeof(fsize);
    p.toBytes(reserve1,raw+idx,sizeof(reserve1)); idx += sizeof(reserve1);
    p.toBytes(reserve2,raw+idx,sizeof(reserve2)); idx += sizeof(reserve2);
    p.toBytes(pixelStart,raw+idx,sizeof(pixelStart)); idx += sizeof(pixelStart);
    ofs.write((char *)raw, BMP::BMPHDRSIZE);
}
// save info header
void BMP::InfoHeader::saveInfo(const BMP &p, std::ofstream &ofs) const {
    unsigned char raw[BMP::BMPINFOSIZE];
    int idx = 0;
    p.toBytes(hdrSize,raw+idx,sizeof(hdrSize)); idx+=sizeof(hdrSize);
    p.toBytes(width,raw+idx,sizeof(width)); idx+=sizeof(width);
    p.toBytes(height,raw+idx,sizeof(height)); idx+=sizeof(height);
    p.toBytes(nPlanes,raw+idx,sizeof(nPlanes)); idx+=sizeof(nPlanes);
    p.toBytes(pixelDepth,raw+idx,sizeof(pixelDepth)); idx+=sizeof(pixelDepth);
    p.toBytes(compressor,raw+idx,sizeof(compressor)); idx+=sizeof(compressor);
    p.toBytes(rawSize,raw+idx,sizeof(rawSize)); idx+=sizeof(rawSize);
    p.toBytes(horizRes,raw+idx,sizeof(horizRes)); idx+=sizeof(horizRes);
    p.toBytes(vertRes,raw+idx,sizeof(vertRes)); idx+=sizeof(vertRes);
    p.toBytes(cmapSize,raw+idx,sizeof(cmapSize)); idx+=sizeof(cmapSize);
    p.toBytes(keyColors,raw+idx,sizeof(keyColors)); idx+=sizeof(keyColors);
    ofs.write((char *)raw, BMP::BMPINFOSIZE);
}

// the pixels
void BMP::savePixels(std::ofstream &ofs) const {
    size_t imgSize = info.getImgSize();
    ofs.seekp(bmpHdr.getOffset(), ofs.beg);
    if (pixels)
        ofs.write((char *) pixels, imgSize);
}

const size_t BMP::InfoHeader::getRowSize() const {
    size_t rowBytes = width * sizeof(Pixel24);
    // pad to 4-byte boundary
    if (rowBytes % 4)
        rowBytes += (4 - rowBytes % 4);
    return rowBytes;
}

// row*height, in bytes
const size_t BMP::InfoHeader::getImgSize() const {
    const size_t rowBytes = (size_t) getRowSize();
    return rowBytes * height;
}
pixel_t *BMP::newPixels() {
    const size_t n = info.getImgSize();
    pixels = new (std::nothrow) pixel_t[n];
    if (pixels)
        memset(pixels, 0, n);
    return pixels;
}

// convert back-n-forth integer to byte-array
// per specification style (really little endian style)
void BMP::toBytes(const uint4 data, unsigned char *p, int n) const {
    while (n-- > 0)
         p[n] = (unsigned char) ((data >> (8 * n)) & 0xff);
}
uint4 BMP::fromBytes(const unsigned char *p, int n) const {
    uint4 data = 0;
    while (n-- > 0)
        data |= ((uint4) p[n]) << (8 * n);
    return data;
}

// overload << to output general info
std::ostream &operator<<(std::ostream &os, const BMP &bmp) {
    os << "File size: " << bmp.bmpHdr.getFSize() << ", " << bmp.info; 
    if (bmp.pixels == nullptr)
        os << std::endl << "no pixel data.";
    return os;
}

std::ostream &operator<<(std::ostream &os, const BMP::InfoHeader &info) {
    os << "Width: " << info.getWidth() << 
        ", Height: " << info.getHeight() <<
        ", Pixel Depth: " << info.pixelDepth << std::endl <<
        "Colormap: " << info.cmapSize <<
        ", Compressor: " << info.compressor << 
        ", Stride Size: " << info.getRowSize() <<
        ", Image Size: " << info.getImgSize();
    return os;
}


}
