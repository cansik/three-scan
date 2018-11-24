//
// Created by Florian Bruggisser on 24.11.18.
//

#ifndef THREE_SCAN_PLYFILE_H
#define THREE_SCAN_PLYFILE_H


#include <WString.h>
#include <model/Vertex.h>
#include "SDCardStorage.h"

class PLYFile {
private:
    typedef SDCardStorage *SDCardStoragePtr;

    String path;

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

    SDCardStoragePtr sd;

    String buffer = "";

    int vertexCount = 0;

public:
    explicit PLYFile(SDCardStoragePtr storage);

    void create(String path);

    void append(Vertex *vertex);

    void close();

    String getData();
};


#endif //THREE_SCAN_PLYFILE_H
