#pragma once
// ============================================================================
// nddpluginapi.h —— NDD 插件 API 标识文件
// ----------------------------------------------------------------------------
// 此文件仅用于标识本项目是一个 NDD 插件，以便与主程序其他
// "ndd-plugin*" 样例保持一致的目录风格。若主程序提供了扩展
// API（例如在 V3.x 中新增的工具函数），请将其声明放在此文件。
// ============================================================================

// 版本号（字符串化，便于 NDD_PROC_IDENTIFY 中引用）
#define NDD_PLUGIN_API_VERSION_MAJOR 1
#define NDD_PLUGIN_API_VERSION_MINOR 0
#define NDD_PLUGIN_API_VERSION_PATCH 0

#define NDD_PLUGIN_API_VERSION_STR "v1.0"
