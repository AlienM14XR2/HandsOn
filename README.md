# Start at 2023-03-10

# HandsOn
C++のメモリ管理とポインタの扱い、その基礎学習、それがきっかけです。

# 開発環境
```
$ lsb_release -a

No LSB modules are available.
Distributor ID:	Ubuntu
Description:	Ubuntu 22.04.2 LTS
Release:	22.04
Codename:	jammy
```
# コンパイラ
```
$ gcc --version

gcc (Ubuntu 11.3.0-1ubuntu1~22.04) 11.3.0
```
諸説意見はあるだろう、しかし、明菜ちゃんはデビュー間もない頃が最高にかわいい。
今は、まだ、ここ :)

# 今後役立つコマンドなど
きっとオレは忘れる。これがあれば、記憶に頼らずに助かるはず。

## e.g. 次の場合、カレントディレクトのファイルに template と記述された箇所を全て表示してくれる。
```
grep template ./*
```
## e.g. 次の場合、カレントディレクト以下のフォルダを再帰的にファイルを検索し、ファイルに template と記述された箇所を全て表示してくれる、はず :)
```
grep -ilr template ./
```
