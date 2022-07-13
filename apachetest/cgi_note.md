# Reference

https://httpd.apache.org/docs/2.4/howto/cgi.html#configuring

# Setting

- Dockerfile

  - python3 の install (python スクリプトを実行するなら)

- dock-compose.yml

  - volume の設定
    - cgi を利用する際のパスは conf_file 内の ScriptAlias ディレクティブ参照
    - conf_file は環境変数で切り替えるよう変更

- conf_file
  - cgi 用の conf_file を複製して下記変更

```
<IfModule !mpm_prefork_module>
        # コメントアウト解除
        LoadModule cgid_module modules/mod_cgid.so
</IfModule>
```

- 実行するファイルの用意

  - test.py

    ```py
    #!/usr/bin/python3
    print("Content-type: text/html")
    print("")
    print("Hello, World")
    ```

  - 権限変更

    ```
    chmod 755 test.py
    ```

# Build & Run

```
export confpath="./conf/cgi.conf"
docker-compose up
```

# Execute

```
curl localhost:8080/cgi-bin/test.py
```
