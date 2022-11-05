from setuptools import setup, Extension
import os.path

def main():
    setup(
        name="pydns",
        version="0.0.1",
        description="Python interface for the dns C library",
        author="b17v134",
        ext_modules=[
            Extension(
                "pydns",
                libraries=['dns'],
                sources = ['dns.c']
                )
        ],
        packages=['dns'],
        package_dir={'dns': os.path.join("src", "dns")}
    )

if __name__ == "__main__":
    main()
