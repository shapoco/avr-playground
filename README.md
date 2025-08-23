# AVR Playground

Microchip (Atmel) AVR に関する雑多なコード

## ビルド環境

このリポジトリでは avr-gcc での開発を前提としており、次のコマンドにパスが通っていることを期待します。

- `avr-g++`
- `avr-objcopy`
- `avr-size`

## 書き込み環境

本リポジトリは WSL での開発を前提にしていますが、USB に関する面倒事の回避のため、以下のような設定を済ませてあることを前提としています。

1. WSL 側 で `avrdude` コマンドが機能するように準備します。
    - 方法例1: Windows 側の avr-gcc の `bin/` フォルダ以下から`avrdude.exe` と `avrdude.conf` をコピーして WSL 側のパスの通った場所に置く
    - 方法例2: 以下のような内容のシェルスクリプトをパスの通ったところに起き、名前を `avrdude` とし、実行権限を与える

        ```sh:avrdude
        #!/bin/bash

        /mnt/c/path/to/avr-gcc/bin/avrdude.exe $@
        ```

2. `avrdude` に渡す、ケーブルに関するパラメータを `AVRDUDE_UPDI_OPTS` 環境変数に設定します。内容は環境に合わせてください。以下は一例です。
    
    ```sh:.bashrc
    export AVRDUDE_UPDI_OPTS="-c serialupdi -P COM7 -x rtsdtr=low"
    ```
