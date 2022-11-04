from setuptools import setup, Extension

def main():
    setup(
        name="dns",
        version="0.0.1",
        description="Python interface for the dns C library",
        author="b17v134",
        ext_modules=[
            Extension(
                "dns",
                libraries=['dns'],
                sources = ['dns.c']
                )
        ],
    )

if __name__ == "__main__":
    main()
