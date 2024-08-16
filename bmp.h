/* Julia Lin
 * jylin@scu.edu
 */
#ifndef BMP_H
#define BMP_H

namespace csenbmp {

// type aliases
using uint4   = uint32_t;     // 4-byte integer
using uint2   = uint16_t;   // 2-byte integer
using pixel_t = uint8_t;  // 1-byte

// 24-bit pixel, 8 for each color channel
struct Pixel24 {
    pixel_t r, g, b;
};

class BMP {
public:
    // https://en.wikipedia.org/wiki/BMP_file_format
    static constexpr char   BMPSignature[] = {'B', 'M'};
    static constexpr size_t BMPHDRSIZE = 14;
    static constexpr size_t BMPINFOSIZE = 40;

    // BMP compressor enumeration. we support only BI_RGB
    enum Compressor {BI_RGB=0, BI_RLE8, BI_RLE4, BI_BITFIELDS5, BI_JPEG, BI_PNG, BI_ALPHABITFIELDS,
        BI_CMYK, BI_CMYKRLE8, BI_CMYKREL4};

    // embedded classes for headers
    class BMPHeader {
        char signature[2];  // must be "BM"
        uint4 fsize;    // file size
        uint2 reserve1;
        uint2 reserve2;
        uint4 pixelStart;   // seek value to the beginning of pixel data
    public:
        BMPHeader(): signature{BMPSignature[0], BMPSignature[1]}, pixelStart(BMPHDRSIZE+BMPINFOSIZE) {}
        const uint4 getFSize(void) const { return fsize; }
        const uint4 getOffset(void) const { return pixelStart;}
        void setFSize(size_t n) { fsize = static_cast<uint4>(n);}
        void setOffset(size_t n) { pixelStart = static_cast<uint4>(n);}
        void load(const BMP &, std::ifstream &);
        void save(const BMP &p, std::ofstream &ofs) const;
        bool acceptSignature() const { return signature[0] == BMPSignature[0] && signature[1] == BMPSignature[1];}
    };
    class InfoHeader {
        uint4 hdrSize;      // support anything >= BMPINFOSIZE, i.e.: 52, 56, 108, 124
        uint4 width;
        uint4 height;
        uint2 nPlanes;      // ignore, supposedly 1
        uint2 pixelDepth;   // support only 24
        uint4 compressor;   // support only BI_RGB == 0
        uint4 rawSize;      // ignore
        uint4 horizRes;     // ignore
        uint4 vertRes;      // ignore
        uint4 cmapSize;     // support only 0
        uint4 keyColors;    // support only 0
    public:
        InfoHeader(): hdrSize(BMPINFOSIZE), width(0), height(0), nPlanes(1), pixelDepth(24),
            compressor(BI_RGB), rawSize(0), horizRes(0), vertRes(0), cmapSize(0), keyColors(0) {}
        void loadInfo(const BMP &, std::ifstream &);
        void saveInfo(const BMP &, std::ofstream &) const;
        inline const uint4 getWidth(void) const { return width;}
        inline const uint4 getHeight(void) const { return height;}
        inline void setWidth(size_t n) { width = static_cast<uint4>(n);}
        inline void setHeight(size_t n) { height = static_cast<uint4>(n);}
        const size_t getRowSize(void) const ;
        const size_t getImgSize(void) const ;
        inline bool acceptInfo() { return hdrSize >= BMPINFOSIZE && pixelDepth == 24 && compressor == BI_RGB && cmapSize == 0; }
        friend std::ostream &operator<<(std::ostream &, const InfoHeader &);
    };
    BMP(): pixels(nullptr) {}
    BMP(const BMP &);   // copy, possible exception
    ~BMP() {if (pixels != nullptr) delete [] pixels;}

    BMP &operator=(const BMP &);    // assignment

    // do not support "move"
    BMP(BMP &&) = delete;
    BMP &operator=(BMP &&) = delete;

    void loadBMP(const std::string &);  // read from a file, possible exception
    void saveBMP(const std::string &) const;     // save to a file

    Pixel24 getPixel(int, int) const;   // get a pixel at location
    void setPixel(int, int, Pixel24);   // set the pixel to value
    pixel_t *getImagePtr(void) { return pixels; }

    pixel_t *newPixels(void);
    BMPHeader bmpHdr;   // file header
    InfoHeader info;    // info header
private:
    pixel_t *pixels;    // pointers to pixels
    void savePixels(std::ofstream &) const;
    void loadPixels(std::ifstream &);
    
    // endian in/out helpers
    uint4 fromBytes(const unsigned char *p, int n) const ;
    void toBytes(const uint4 data, unsigned char *p, int n) const ;

public:
    friend std::ostream &operator<<(std::ostream &, const BMP &);
};


// new filename not to clash with old
std::string genNewFname(const std::string);

}

#endif  // BMP_H