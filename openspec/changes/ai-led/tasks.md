## 1. ESP32 固件开发

- [x] 1.1 创建 Arduino 项目，配置 ESP32C3 开发板
- [x] 1.2 实现 BLE GATT 服务和特征值
- [x] 1.3 实现 LED 引脚定义和灯效控制（红/黄/绿）
- [x] 1.4 实现蜂鸣器控制逻辑
- [x] 1.5 实现状态命令解析和状态机
- [x] 1.6 添加串口调试输出
- [ ] 1.7 测试固件：通过蓝牙调试工具发送命令验证

## 2. PowerShell 桥接脚本开发

- [x] 2.1 创建 PowerShell 脚本框架，添加参数解析
- [x] 2.2 实现 BLE 设备扫描和连接逻辑
- [x] 2.3 实现状态命令映射和发送
- [x] 2.4 添加连接超时和错误处理
- [ ] 2.5 测试脚本：手动执行验证蓝牙通信

## 3. Claude Code 集成

- [ ] 3.1 配置 Claude Code settings.json，添加 hooks
- [ ] 3.2 测试 SessionStart/Stop 事件
- [ ] 3.3 测试 PreToolUse/PostToolUse 事件
- [ ] 3.4 测试 PermissionRequest 事件
- [ ] 3.5 验证完整工作流程
