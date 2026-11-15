## Linux 安装

### Debian 系发行版

使用 Debian 系发行版时，推荐使用 .deb 包安装：

```shell
sudo apt install ./nekoray-*-debian-x64.deb
```

安装完成后，桌面快捷方式启动自带参数 `-appdata`，如果想要直接启动并使用之前的配置，注意附带本参数。

### Arch 系发行版

使用 Arch 系发行版时，推荐从 ```aur``` 或 ```archlinuxcn``` 安装：

#### AUR
##### 最新稳定版

```shell
[yay/paru] -S nekoray
```

##### 最新 Git 版 (开发版)

```shell
[yay/paru] -S nekoray-git
```

#### archlinuxcn

##### 最新稳定版

```shell
sudo pacman -S nekoray
```

##### 最新 Git 版 (开发版)

```shell
sudo pacman -S nekoray-git
```

### 其他发行版

下载 .zip 文件，解压到合适的路径，开箱即用。

或下载 .AppImage，并使用 `chmod +x nekoray-*-AppImage-x64.AppImage` 给予可执行权限。

具体使用方法见下文。

## Linux 运行

### Repeated DNS authorization prompts with systemd-resolved

When TUN starts as a normal user, systemd-resolved may ask separately for
permission to set DNS servers, DNS routing domains, and the default DNS route.
The core's `cap_net_admin` capability does not authorize these D-Bus operations.

To install an optional policy for your current desktop account, run from the
project directory **without sudo**:

```bash
bash libs/install_linux_dns_policy.sh
```

Approve the single installation prompt, then restart the profile. This permits
only the three DNS operations on `neko-tun`, while your account has an active
local session. It also permits other processes in that session to perform those
same operations on `neko-tun`; it does not authorize changes on Wi-Fi or Ethernet.
The systemd-resolved version must expose the `interface` polkit detail (verified
with systemd 259). Versions without it retain their normal authorization prompts.
No application rebuild or polkit restart is needed.

To remove the policy and restore normal authorization:

```bash
bash libs/install_linux_dns_policy.sh --remove
```

**使用 Linux 系统相信您已具备基本的排错能力，
本项目不提供特定发行版/架构的支持，预编译文件不能满足您的需求时，请自行编译/适配。**

已知部分 Linux 发行版无法使用、非 x86_64 暂无适配，可以尝试自行编译。

目前 Release 便携包解压后，有两种使用方法：

1. System: 若要使用系统的 Qt5 运行库，请执行 `./nekoray`
2. Bundle: 若要使用预编译的 Qt 运行库，请执行 `./launcher`

### Bundle

要求：已安装主流的发行版和 xcb 桌面环境。

运行： `./launcher` 或 部分系统可双击打开

launcher 参数

* `./launcher -- -appdata` ( `--` 后的参数传递给主程序 )
* `-debug` Debug mode

Ubuntu 22.04: `sudo apt install libxcb-xinerama0`

### System

要求：已安装主流的发行版和 xcb 桌面环境，已安装 Qt5.12 ~ Qt5.15 环境。

运行： `./nekoray` 或 部分系统可双击打开。如果无法运行，建议使用 Bundle 版。
