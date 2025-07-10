My personal project used to create ESP32_Resource_Monitor component for esp-idf
It can be used to template for a project .
The device(board)used is Lilygo T HHMI S3 with 16 MB Flash and 8 MB PSRAM.

to clone :
```
git clone https://github.com/florinbaciuu/lilygo-thmi-idf-template-project

git submodule status                                              # Verificare submodule
git submodule update --init --recursive                           # Inițializare + update
```


or to clone complete with submodules
```
git clone --recurse-submodules https://github.com/florinbaciuu/lilygo-thmi-idf-template-project
```