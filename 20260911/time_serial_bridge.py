import argparse
import json
import time
import urllib.request
from datetime import datetime


API_URL = "https://utctime.app/api/now/Asia/Shanghai"
BAUD_RATE = 115200


def scan_serial_ports():
    try:
        from serial.tools import list_ports
    except ImportError as exc:
        raise RuntimeError(
            "缺少 pyserial，请先运行: python -m pip install -r requirements.txt"
        ) from exc

    return sorted(port.device for port in list_ports.comports())


def select_serial_port(requested_port=None):
    ports = scan_serial_ports()
    if not ports:
        raise RuntimeError("未扫描到 COM 串口，请检查 USB 转串口连接和驱动。")

    print("扫描到串口: " + ", ".join(ports))

    if requested_port:
        requested_port = requested_port.upper()
        available = {port.upper(): port for port in ports}
        if requested_port not in available:
            raise RuntimeError(
                f"指定的串口 {requested_port} 不存在。当前串口: {', '.join(ports)}"
            )
        return available[requested_port]

    if len(ports) == 1:
        return ports[0]

    raise RuntimeError(
        "扫描到多个串口，请指定 STM32 所在端口，例如: "
        "python time_serial_bridge.py --port COM3"
    )


def fetch_datetime():
    request = urllib.request.Request(
        API_URL,
        headers={"User-Agent": "STM32-Time-Serial-Bridge/1.0"},
    )
    with urllib.request.urlopen(request, timeout=8) as response:
        data = json.load(response)

    value = data.get("datetime")
    if not value:
        raise RuntimeError("接口返回中没有 datetime 字段。")

    return datetime.fromisoformat(value).strftime("%Y-%m-%d\n%H:%M:%S")


def build_payload(datetime_text):
    message = json.dumps(
        {"datetime": datetime_text},
        ensure_ascii=True,
        separators=(",", ":"),
    )
    return (message + "\r\n").encode("ascii")


def run(port_name, interval_seconds):
    try:
        import serial
    except ImportError as exc:
        raise RuntimeError(
            "缺少 pyserial，请先运行: python -m pip install -r requirements.txt"
        ) from exc

    with serial.Serial(
        port=port_name,
        baudrate=BAUD_RATE,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        timeout=1,
        write_timeout=2,
        xonxoff=False,
        rtscts=False,
        dsrdtr=False,
    ) as uart:
        print(f"已连接 {port_name}，参数 115200 / 8N1 / 无流控。按 Ctrl+C 停止。")

        while True:
            started_at = time.monotonic()
            try:
                payload = build_payload(fetch_datetime())
                uart.write(payload)
                uart.flush()
                print(
                    f"[{time.strftime('%H:%M:%S')}] 已发送: "
                    f"{payload.decode('ascii').rstrip()}"
                )
            except Exception as exc:
                print(f"警告: 本次获取或发送失败: {exc}")

            remaining = interval_seconds - (time.monotonic() - started_at)
            if remaining > 0:
                time.sleep(remaining)


def parse_args():
    parser = argparse.ArgumentParser(description="定时获取北京时间并发送到 STM32 串口")
    parser.add_argument("--port", help="STM32 串口，例如 COM3；只有一个串口时可省略")
    parser.add_argument(
        "--interval",
        type=float,
        default=10,
        help="发送间隔秒数，默认 10 秒",
    )
    parser.add_argument(
        "--preview",
        action="store_true",
        help="只获取并显示一次数据，不打开串口",
    )
    args = parser.parse_args()
    if args.interval <= 0:
        parser.error("--interval 必须大于 0")
    return args


def main():
    args = parse_args()
    if args.preview:
        print(build_payload(fetch_datetime()).decode("ascii").rstrip())
        return

    port_name = select_serial_port(args.port)
    run(port_name, args.interval)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n程序已停止，串口已关闭。")
    except Exception as exc:
        print(f"错误: {exc}")
        raise SystemExit(1)
