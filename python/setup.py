from setuptools import setup, Extension
import os.path

def main():
    dir_path = os.path.dirname(os.path.realpath(__file__))
    setup(
        name="pydns",
        version="0.0.1",
        description="Python interface for the dns C library",
        author="b17v134",
        ext_modules=[
            Extension(
                "pydns",
                libraries=['dns'],
                sources = ['dns.c'],
                include_dirs=[dir_path + '/../include'],
                library_dirs=[dir_path + '/../src/.libs'],
                )
        ],
        packages=['dns'],
        package_dir={'dns': os.path.join("src", "dns")}
    )

if __name__ == "__main__":
    main()
