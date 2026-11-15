## 构建 nekobox_core

### 目录结构

```
  | nekoray
  |   go/cmd/*
  | sing-box-extra
  | sing-box
  | ......
```

### 常规构建

Use Go 1.22.x, matching the CI build. The pinned networking dependencies use
internal Go symbols that newer Go linkers reject (for example,
`invalid reference to net.errNoSuchInterface`). To use an isolated Go 1.22
installation, prepend its `bin` directory to `PATH` when running
`libs/build_go.sh`; the system Go installation does not need to be replaced.

1. `bash libs/get_source.sh` （自动下载目录结构，自动 checkout commit）
2. `GOOS=windows GOARCH=amd64 bash libs/build_go.sh`

具体支持的 GOOS 和 GOARCH 请看 `libs/build_go.sh`

非官方构建无需编译 `updater` `launcher`

### sing-box tags

具体使用的 tags 请看 `libs/build_go.sh`
