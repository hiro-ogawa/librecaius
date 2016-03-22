# librecaius
東芝の音声認識，音声合成システムRECAIUSのWebAPIを使用するためのライブラリ群です．
残念ながら全ての機能が実装されているわけではありません．

## 依存ライブラリ
* Boost
  * XMLのパースに使用
* libcurl
  * REST APIへのアクセスに使用
* libjsoncpp
  * jsonのパースに使用

## コンパイル
buildディレクトリで下記の通りで実行できます．

``` Bash
cmake ..
make
make run
```

## 注意
build内の設定ファイル_recaius_id.jsonには正しい情報を記載し，
ファイル名の先頭のアンダーバーをなくしてrecaius_id.jsonにする必要があります．

## License
This software is released under the MIT License, see LICENSE.txt.
