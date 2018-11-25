//
// Created by Florian Bruggisser on 25.11.18.
//

#include <util/StatusLed.h>
#include "CloudFile.h"

CloudFile::CloudFile(CloudFile::SDCardStoragePtr storage) {
    this->storage = storage;
}

void CloudFile::create(const String &path) {
    this->path = path;
    vertexCounter = 0;

    openFile(FILE_WRITE);
    writeHeader();
    closeFile();
}

void CloudFile::appendBuffer(MutableBuffer<Vertex> &vertices) {
    openFile(FILE_APPEND);
    for (unsigned int i = 0; i < vertices.length(); i++) {
        appendSingle(vertices.get(i));
        vertexCounter++;
    }
    closeFile();
}

void CloudFile::close() {
    openFile(FILE_APPEND);
    file.printf("vertices: %d", vertexCounter);
    closeFile();
}

void CloudFile::openFile(const char *mode) {
    storage->secureMount();
    file = SD.open(path.c_str(), mode);
}

void CloudFile::closeFile() {
    file.close();
    storage->unmount();
}

void CloudFile::writeHeader() {
    file.print(headerTemplate);
}

void CloudFile::appendSingle(CloudFile::VertexPtr vertex) {
    auto position = vertex->getSphericalPosition();
    auto color = vertex->getSignalStrength();

    String m = String(position.x, 4) + " " + String(position.y, 4) + " " + String(position.z, 4)
               + " " + String(color) + " " + String(color) + " " + String(color) + "\n";
    file.print(m);
}
