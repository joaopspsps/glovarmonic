Import("env")  # type: ignore


def before_littlefs(source, target, env):
    import gzip
    import runpy
    import shutil
    from pathlib import Path

    project_dir = Path(env.get("PROJECT_DIR"))

    # Source virtualenv in project_dir where dependencies are installed
    runpy.run_path(project_dir / ".venv/bin/activate_this.py")

    import esptool
    import minify_html

    # Helper functions
    # ================

    def get_glovarmonic_id() -> str:
        """The glovarmonic_id is the lowest 3 bytes of the board's MAC address."""
        esp = esptool.detect_chip(port="/dev/ttyUSB0")
        mac = esp.read_mac()
        mac_str = "".join([f"{byte:02x}" for byte in mac[:3][::-1]])
        return mac_str

    # AsserBuilder Passes
    # ===================

    def pass_process_templates(html: str, glovarmonic_id: str) -> str:
        html = html.replace("{{ glovarmonic_id }}", glovarmonic_id)
        return html

    def pass_minify_html(html: str) -> str:
        return minify_html.minify(
            html, minify_css=True, minify_js=True, keep_input_type_text_attr=True
        )

    def pass_compress(data: bytes) -> bytes:
        return gzip.compress(data)

    # AssetBuilder classes
    # ====================

    class AssetBuilder:
        def __init__(self, read_text=True, write_text=True, passes=None) -> None:
            self.passes = [] if passes is None else passes
            self.read_text = read_text
            self.write_text = write_text

        def build(self, src: Path, dst: Path) -> None:
            try:
                dst.parent.mkdir(parents=True)
            except FileExistsError:
                pass

            if self.read_text:
                data = src.read_text()
            else:
                data = src.read_bytes()

            size_start = len(data)
            for p in self.passes:
                data = p(data)

            if self.write_text:
                size_end = dst.write_text(data)
            else:
                size_end = dst.write_bytes(data)

            src_short = str(src).removeprefix(str(project_dir))
            dst_short = str(dst).removeprefix(str(project_dir))
            print(
                f"prepare_data: {src_short} -> {dst_short} ({size_start / 1024:.2f}KB -> {size_end / 1024:.2f}KB)"
            )

    class TextBuilder(AssetBuilder):
        pass

    class BytesBuilder(AssetBuilder):
        def __init__(self, **kwargs):
            super().__init__(read_text=False, write_text=False, **kwargs)

    class CompressBuilder(BytesBuilder):
        def __init__(self):
            super().__init__(passes=[lambda data: pass_compress(data)])

    class HTMLBuilder(AssetBuilder):
        def __init__(self, **kwargs):
            glovarmonic_id = get_glovarmonic_id()
            super().__init__(
                write_text=False,
                passes=[
                    lambda data: pass_process_templates(data, glovarmonic_id),
                    lambda data: pass_minify_html(data),
                    lambda data: pass_compress(data.encode()),
                ],
            )

    # Setup directories
    # =================

    path_assets = project_dir / "assets"
    if not path_assets.exists():
        print(f"{path_assets.name} directory does not exist; not doing anything")
        return

    path_data = project_dir / "data"
    if path_data.exists():
        # Remove directory or file
        if path_data.is_dir():
            shutil.rmtree(path_data)
        else:
            path_data.unlink()
    # Create new empty data dir
    path_data.mkdir(parents=True)

    # Build data dir
    # ==============

    text_builder = TextBuilder()
    bytes_builder = BytesBuilder()
    compress_builder = CompressBuilder()
    html_builder = HTMLBuilder()

    for builder, dir in zip((html_builder, compress_builder), ("html", "js")):
        for src in (path_assets / dir).glob(f"*.{dir}"):
            if src.is_file():
                dst = Path(str(path_data / dir / src.name) + ".gz")
                builder.build(src, dst)

    for filename in (
        "Doto/VariableFont_ROND,wght.woff2",
        "Roboto/Regular.woff2",
    ):
        bytes_builder.build(
            path_assets / "font" / filename, path_data / "font" / filename
        )

    for filename in ("wifi_ssid", "wifi_psk"):
        src = path_assets / filename
        if src.is_file():
            text_builder.build(src, path_data / filename)


env.AddPreAction("$BUILD_DIR/LittleFS.bin", before_littlefs)  # type: ignore
