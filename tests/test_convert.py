"""
Test Harness for StringsToSqlListPlugin — 核心转换逻辑验证
----------------------------------------------------------------

本脚本用 Python 模拟 C++ 中 convertByComma / convertByNewLine /
convertAdvanced 的行为，并对关键用例做断言。用于在不依赖 Qt
与 QScintilla 的情况下，离线验证字符串处理是否符合预期。

运行：python3 test_convert.py
"""
import re


def split_by_comma_family(text: str):
    """模拟 C++ 中的 [,、;；\t]+ 正则分割"""
    parts = re.split(r'[,、;；\t]+', text)
    return parts


def split_by_newline(text: str):
    """模拟 C++ 中的 \r?\n 分割（先归一化 \r\n -> \n）"""
    normalized = text.replace('\r\n', '\n').replace('\r', '\n')
    return normalized.split('\n')


def do_convert(parts, trim_items=True, skip_empty=True,
               quote_char="'", wrap_parens=True, joiner=","):
    """与 C++ StringsToSqlListPlugin::doConvert 对应"""
    out = []
    for item in parts:
        if trim_items:
            item = item.strip()
        if skip_empty and item == "":
            continue
        if quote_char:
            # SQL 约定：item 内部的引号"加倍"
            escaped = item.replace(quote_char, quote_char + quote_char)
            item = f"{quote_char}{escaped}{quote_char}"
        out.append(item)

    body = (joiner + " ").join(out)
    if wrap_parens:
        body = f"({body})"
    return body


def convert_by_comma(text: str) -> str:
    return do_convert(split_by_comma_family(text))


def convert_by_newline(text: str) -> str:
    return do_convert(split_by_newline(text))


# ----------------------------------------------------------------------------
# 测试用例
# ----------------------------------------------------------------------------
def test_basic_comma():
    assert convert_by_comma("aaa, bbb, ccc") == "('aaa', 'bbb', 'ccc')"
    assert convert_by_comma("aaa、bbb、ccc") == "('aaa', 'bbb', 'ccc')"
    assert convert_by_comma("aaa;bbb;ccc")  == "('aaa', 'bbb', 'ccc')"
    assert convert_by_comma("aaa; ； bbb\tccc") == "('aaa', 'bbb', 'ccc')"
    print("[OK] test_basic_comma")


def test_quote_escaping():
    # 单引号内部 -> 加倍
    assert convert_by_comma("It's, O'Neil") == "('It''s', 'O''Neil')"
    print("[OK] test_quote_escaping")


def test_newline_split():
    text = "apple\nbanana\ncherry"
    assert convert_by_newline(text) == "('apple', 'banana', 'cherry')"
    # 空行跳过
    text = "apple\n\nbanana\n  \ncherry"
    assert convert_by_newline(text) == "('apple', 'banana', 'cherry')"
    # Windows 换行 \r\n
    text = "a\r\nb\r\nc"
    assert convert_by_newline(text) == "('a', 'b', 'c')"
    print("[OK] test_newline_split")


def test_single_item():
    assert convert_by_comma("only") == "('only')"
    print("[OK] test_single_item")


def test_double_quote_mode():
    parts = split_by_comma_family("1, 2, 3")
    got = do_convert(parts, quote_char='"')
    assert got == '("1", "2", "3")'
    print("[OK] test_double_quote_mode")


def test_no_quote_no_parens():
    parts = split_by_comma_family("1, 2, 3")
    got = do_convert(parts, quote_char="", wrap_parens=False)
    assert got == "1, 2, 3"
    print("[OK] test_no_quote_no_parens")


def test_custom_delimiter():
    parts = "aaa | bbb | ccc".split("|")
    got = do_convert(parts)
    assert got == "('aaa', 'bbb', 'ccc')"
    print("[OK] test_custom_delimiter")


def test_custom_joiner():
    parts = split_by_comma_family("a, b, c")
    got = do_convert(parts, joiner=" |")
    # 连接符 + 空格 -> " | "
    assert got == "('a' | 'b' | 'c')"
    print("[OK] test_custom_joiner")


def test_leading_and_trailing_noise():
    # 首尾分隔符 => 多出空项 -> 被 skip_empty 吃掉
    assert convert_by_comma(",,aaa,,bbb,,") == "('aaa', 'bbb')"
    print("[OK] test_leading_and_trailing_noise")


def test_double_quote_internal_escape():
    # 若选双引号作为包裹字符，则内容中的 " 被加倍
    parts = split_by_comma_family('a"b, cd')
    got = do_convert(parts, quote_char='"')
    assert got == '("a""b", "cd")'
    print("[OK] test_double_quote_internal_escape")


def test_whitespace_only_items_are_trimmed_to_empty_and_skipped():
    text = "  ,  ,  hello  ,  "
    parts = split_by_comma_family(text)
    # trim 后都成为空，跳过；只剩 'hello'
    got = do_convert(parts, trim_items=True, skip_empty=True)
    assert got == "('hello')"
    print("[OK] test_whitespace_only_items_are_trimmed_to_empty_and_skipped")


if __name__ == "__main__":
    test_basic_comma()
    test_quote_escaping()
    test_newline_split()
    test_single_item()
    test_double_quote_mode()
    test_no_quote_no_parens()
    test_custom_delimiter()
    test_custom_joiner()
    test_leading_and_trailing_noise()
    test_double_quote_internal_escape()
    test_whitespace_only_items_are_trimmed_to_empty_and_skipped()
    print("\nAll tests passed ✅")
