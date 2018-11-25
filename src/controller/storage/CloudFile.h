//
// Created by Florian Bruggisser on 25.11.18.
//

#ifndef THREE_SCAN_CLOUDFILE_H
#define THREE_SCAN_CLOUDFILE_H


#include <model/Vertex.h>
#include <WString.h>
#include <util/MutableBuffer.h>
#include "SDCardStorage.h"

class CloudFile {
private:
    void openFile(const char *mode);

    void closeFile();

protected:
    typedef MutableBuffer<Vertex> *VertexBuffer;
    typedef Vertex *VertexPtr;
    typedef SDCardStorage *SDCardStoragePtr;

    File file;

    String path;

    SDCardStoragePtr storage;

    const String headerTemplate = "ply\n"
                                  "format ascii 1.0\n"
                                  "comment Created by Three Scan (Florian Bruggisser)\n"
                                  "obj_info Generated by Three Scan\n"
                                  "element vertex %vertexCount\n"
                                  "property float x\n"
                                  "property float y\n"
                                  "property float z\n"
                                  "property uchar red\n"
                                  "property uchar green\n"
                                  "property uchar blue\n"
                                  "end_header\n";

    unsigned int vertexCounter;

    virtual void writeHeader();

    virtual void appendSingle(VertexPtr vertex);


public:
    explicit CloudFile(SDCardStoragePtr storage);

    void create(const String &path);

    void appendBuffer(MutableBuffer<Vertex> &vertices);

    void close();
};


#endif //THREE_SCAN_CLOUDFILE_H
