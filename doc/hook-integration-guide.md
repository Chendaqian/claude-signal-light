# AI 编程工具 Hook 集成指南

本指南介绍如何使用 `send-hook.py` 脚本将 Claude-LED 状态指示灯接入主流 AI 编程工具的 hook 系统，实现工作状态的物理提醒。

## 目录

- [前置条件](#前置条件)
- [send-hook.py 脚本说明](#send-hookpy-脚本说明)
- [Claude Code 集成](#claude-code-集成)
- [Codex 集成](#codex-集成)
- [Trae 集成](#trae-集成)
- [Cursor 集成](#cursor-集成)
- [推荐灯效映射](#推荐灯效映射)
- [故障排除](#故障排除)

---

## 前置条件

### 硬件准备

| 组件 | 型号/规格 |
|------|----------|
| 开发板 | ESP32C3-Super-Mini |
| LED 电路板 | 三色 LED（共阴极，红/黄/绿） |
| 蜂鸣器 | 无源蜂鸣器模块（3线制：VCC、IO、GND） |

### 软件依赖

```bash
# 安装 Python BLE 库
pip install bleak
```

### 固件烧录

用 Arduino IDE 打开 `firmware/claude-led/claude-led.ino`，选择 ESP32C3 开发板，烧录固件。

### 验证连接

```bash
# 测试全部灯效（每种 3 秒）
python scripts/test-all-effects.py

# 测试单个灯效
python scripts/send-hook.py 17        # 太极呼吸
python scripts/send-hook.py alarm 1   # 红灯常亮 + 短响一声
```

---

## send-hook.py 脚本说明

### 基本用法

```bash
python send-hook.py <mode> [buzzer_param]
```

### 参数说明

| 参数 | 说明 | 取值范围 |
|------|------|----------|
| `mode` | 灯效模式 | 0-19 的数字或名称字符串 |
| `buzzer_param` | 蜂鸣器参数（可选，默认 0） | 0=不响，1=短响一声，2=短响两声，3=高频短响 |

### 灯效模式速查表

| 模式 | 名称 | 说明 | 适用场景 |
|------|------|------|----------|
| 0 | off | 全灭 | 空闲/会话结束 |
| 2 | thinking | 绿灯闪烁 | AI 思考中/工具调用中 |
| 3 | error | 红灯闪烁 | 错误/失败 |
| 5 | success | 绿灯常亮 | 任务完成 |
| 6 | alarm | 红灯常亮 | 需要用户关注/权限请求 |
| 7 | busy | 黄灯常亮 | 忙碌/执行中 |
| 17 | taichi | 太极呼吸 | 会话开始/运行中 |
| 18 | hello | HELLO 摩尔斯码 | 首次启动/欢迎 |
| - | castle / sky | 天空之城短版 (~2.5s) | 彩蛋/启动音 |
| - | castle,1 / sky,1 | 天空之城完整版 (~46s) | 彩蛋 |

### 路径配置

在配置文件中使用脚本的**绝对路径**，例如：

- Windows: `python "D:\Source\Repos\claude-signal-light\scripts\send-hook.py"`
- Linux/macOS: `python3 "/path/to/claude-signal-light/scripts/send-hook.py"`

---

## Claude Code 集成

### 配置文件位置

| 级别 | 路径 |
|------|------|
| 全局配置 | `~/.claude/settings.json` |
| 项目级配置 | `.claude/settings.json`（项目根目录） |

### 支持的事件类型

| 事件 | 触发时机 | 推荐灯效 |
|------|----------|----------|
| `SessionStart` | 会话开始 | 17（太极呼吸） |
| `UserPromptSubmit` | 用户提交提示 | 2（绿灯闪烁） |
| `PreToolUse` | 工具执行前 | 2（绿灯闪烁） |
| `PostToolUse` | 工具执行后 | 2（绿灯闪烁） |
| `PostToolUseFailure` | 工具执行失败 | 3（红灯闪烁） |
| `PermissionRequest` | 请求用户权限 | 6（红灯常亮）+ 蜂鸣器 |
| `Stop` | AI 即将停止响应 | 5（绿灯常亮） |
| `Notification` | AI 发送通知 | 7（黄灯常亮） |
| `SessionEnd` | 会话结束 | 0（全灭） |

### 配置示例

在 `~/.claude/settings.json` 中添加：

```json
{
  "hooks": {
    "SessionStart": [
      {
        "matcher": ".*",
        "hooks": [
          {
            "type": "command",
            "command": "python \"D:\\Source\\Repos\\claude-signal-light\\scripts\\send-hook.py\" 17",
            "timeout": 5000
          }
        ]
      }
    ],
    "PreToolUse": [
      {
        "matcher": ".*",
        "hooks": [
          {
            "type": "command",
            "command": "python \"D:\\Source\\Repos\\claude-signal-light\\scripts\\send-hook.py\" 2",
            "timeout": 5000
          }
        ]
      }
    ],
    "PostToolUseFailure": [
      {
        "matcher": ".*",
        "hooks": [
          {
            "type": "command",
            "command": "python \"D:\\Source\\Repos\\claude-signal-light\\scripts\\send-hook.py\" 3",
            "timeout": 5000
          }
        ]
      }
    ],
    "PermissionRequest": [
      {
        "matcher": ".*",
        "hooks": [
          {
            "type": "command",
            "command": "python \"D:\\Source\\Repos\\claude-signal-light\\scripts\\send-hook.py\" 6 1",
            "timeout": 5000
          }
        ]
      }
    ],
    "Stop": [
      {
        "matcher": ".*",
        "hooks": [
          {
            "type": "command",
            "command": "python \"D:\\Source\\Repos\\claude-signal-light\\scripts\\send-hook.py\" 5",
            "timeout": 5000
          }
        ]
      }
    ],
    "SessionEnd": [
      {
        "matcher": ".*",
        "hooks": [
          {
            "type": "command",
            "command": "python \"D:\\Source\\Repos\\claude-signal-light\\scripts\\send-hook.py\" 0",
            "timeout": 5000
          }
        ]
      }
    ]
  }
}
```

### matcher 说明

- `".*"` - 匹配所有工具
- `"Bash"` - 仅匹配 Bash 命令
- `"Write"` - 仅匹配文件写入
- `"Edit"` - 仅匹配文件编辑

---

## Codex 集成

Codex 是 OpenAI 的 CLI 编程工具，支持简单的 hook 机制。

### 配置文件位置

| 级别 | 路径 |
|------|------|
| 全局配置 | `~/.codex/config.json` |
| 项目级配置 | `.codex/config.json` |

### 支持的事件类型

| 事件 | 触发时机 |
|------|----------|
| `pre_command` | 命令执行前 |
| `post_command` | 命令执行后 |

### 配置示例

在 `~/.codex/config.json` 中添加：

```json
{
  "hooks": {
    "pre_command": "python \"D:\\Source\\Repos\\claude-signal-light\\scripts\\send-hook.py\" 2",
    "post_command": "python \"D:\\Source\\Repos\\claude-signal-light\\scripts\\send-hook.py\" 5"
  }
}
```

### 注意事项

- Codex 的 hook 配置相对简单，直接指定脚本路径
- 建议将脚本设置为可执行（Linux/macOS）：`chmod +x send-hook.py`
- 主要用于安全审批场景，事件类型较少

---

## Trae 集成

Trae 是字节跳动推出的 AI 编程工具，支持 Agent 模式下的 hook 机制。

### 配置文件位置

- 项目配置：`trae.json` 或 `trae_agent.json`（项目根目录）

### 支持的事件类型

| 事件 | 触发时机 |
|------|----------|
| `preToolCall` | 工具调用前 |
| `postToolCall` | 工具调用后 |

### 配置示例

在项目根目录的 `trae.json` 中添加：

```json
{
  "hooks": {
    "preToolCall": [
      {
        "command": "python \"D:\\Source\\Repos\\claude-signal-light\\scripts\\send-hook.py\" 2"
      }
    ],
    "postToolCall": [
      {
        "command": "python \"D:\\Source\\Repos\\claude-signal-light\\scripts\\send-hook.py\" 5"
      }
    ]
  }
}
```

### MCP 扩展支持

Trae 支持 MCP（Model Context Protocol）协议扩展，可以通过 MCP 服务器实现更灵活的 hook 集成。

---

## Cursor 集成

Cursor 是一款流行的 AI 编程 IDE，内置 hook 支持。

### 配置方式

#### 方式一：配置文件

在 `.cursor/settings.json` 中添加：

```json
{
  "hooks": {
    "before_apply": [
      {
        "command": "python \"D:\\Source\\Repos\\claude-signal-light\\scripts\\send-hook.py\" 2",
        "description": "AI 应用代码更改前 - 绿灯闪烁"
      }
    ],
    "after_apply": [
      {
        "command": "python \"D:\\Source\\Repos\\claude-signal-light\\scripts\\send-hook.py\" 5",
        "description": "AI 应用代码更改后 - 绿灯常亮"
      }
    ]
  }
}
```

#### 方式二：设置 UI

1. 打开 Cursor 设置（`Ctrl + ,`）
2. 导航到 **Features** 或 **Cursor** 选项卡
3. 找到 Hooks 相关配置项
4. 添加 before_apply 和 after_apply 命令

### 支持的事件类型

| 事件 | 触发时机 |
|------|----------|
| `before_apply` | AI 应用代码更改前 |
| `after_apply` | AI 应用代码更改后 |

---

## 推荐灯效映射

### 通用映射方案

| 工具状态 | 灯效模式 | 说明 |
|----------|----------|------|
| 空闲/待命 | 0（全灭） | 无操作状态 |
| AI 思考中 | 2（绿灯闪烁） | 处理请求 |
| 工具执行中 | 2（绿灯闪烁） | 调用外部工具 |
| 任务完成 | 5（绿灯常亮） | 成功完成 |
| 执行失败 | 3（红灯闪烁） | 出现错误 |
| 需要关注 | 6（红灯常亮）+ 蜂鸣器 | 权限请求/等待输入 |
| 会话开始 | 17（太极呼吸） | 启动动画 |
| 会话结束 | 0（全灭） | 关闭 |

### 蜂鸣器使用建议

| 场景 | 蜂鸣器参数 | 说明 |
|------|------------|------|
| 权限请求 | 1（短响一声，2000Hz） | 提醒用户注意 |
| 严重错误 | 2（短响两声，2000Hz+1000Hz） | 强调错误 |
| 高频提示 | 3（高频短响，4000Hz） | 醒目提醒 |
| 其他场景 | 0（不响） | 静默模式 |

### 彩蛋：天空之城

开机自动播放天空之城前奏短音（~2.5秒）。也可手动触发：

```bash
python scripts/send-hook.py castle      # 短版 (~2.5s)
python scripts/send-hook.py castle,1    # 完整版 (~46s)
python scripts/send-hook.py sky         # 同 castle
```

播放中再次发送 `castle` 可停止。旋律由 8051 版本移植。

### Claude Code 完整映射表

| Hook 事件 | 灯效参数 | 灯效名称 | 蜂鸣器 |
|-----------|----------|----------|--------|
| SessionStart | 17 | 太极呼吸 | 0 |
| UserPromptSubmit | 2 | 绿灯闪烁 | 0 |
| PreToolUse | 2 | 绿灯闪烁 | 0 |
| PostToolUse | 2 | 绿灯闪烁 | 0 |
| PostToolUseFailure | 3 | 红灯闪烁 | 0 |
| PermissionRequest | 6 | 红灯常亮 | 1 |
| Stop | 5 | 绿灯常亮 | 0 |
| Notification | 7 | 黄灯常亮 | 0 |
| SessionEnd | 0 | 全灭 | 0 |

---

## 故障排除

### 常见问题

#### 1. 设备未找到

**现象**：脚本提示 `Device not found`

**解决方案**：
- 确认 ESP32 已通电并运行固件
- 确认电脑蓝牙已开启
- 检查设备名是否为 `Claude-LED-LUCKEY`
- 尝试重新烧录固件

#### 2. 连接超时

**现象**：脚本提示 `Connection failed`

**解决方案**：
- 缩短 ESP32 与电脑的距离
- 检查是否有其他设备已连接
- 重启 ESP32 后重试

#### 3. Hook 未触发

**现象**：AI 工具事件未触发灯效变化

**解决方案**：
- 检查配置文件路径是否正确
- 确认 JSON 格式无语法错误
- 检查脚本路径是否使用绝对路径
- Windows 路径使用双反斜杠：`D:\\path\\to\\script`
- 查看日志文件：`%TEMP%\Claude-LED-LUCKEY\python-hook.log`

#### 4. Python 环境问题

**现象**：`ModuleNotFoundError: No module named 'bleak'`

**解决方案**：
```bash
pip install bleak
```

### 日志查看

脚本运行日志保存在临时目录：

- Windows: `%TEMP%\Claude-LED-LUCKEY\python-hook.log`
- Linux/macOS: `/tmp/Claude-LED-LUCKEY/python-hook.log`

### 手动测试

```bash
# 测试基本连接
python scripts/send-hook.py 2

# 测试带蜂鸣器
python scripts/send-hook.py 6 1
python scripts/send-hook.py alarm 3   # 高频提示音

# 测试天空之城
python scripts/send-hook.py castle    # 短版
python scripts/send-hook.py castle,1  # 完整版

# 测试全部灯效
python scripts/test-all-effects.py
```

---

