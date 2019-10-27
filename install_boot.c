#include <stdio.h>

char buffer[1024];

int main(int argc, char **argv)
{
	FILE *fp;
	if (argc<3) {
		printf("Sintax: %s <boot> <image>\n", argv[0]);
		return 1;
	}

	fp=fopen(argv[1], "rb");
	if (!fp) {
		printf("Cannot open %s\n", argv[1]);
		return 1;
	}

	fread(buffer, 1024, 1, fp);

	fclose(fp);

	fp=fopen(argv[2], "r+b");
	if (!fp) {
		printf("Cannot open %s\n", argv[2]);
		return 1;
	}
	fwrite(buffer, 1024, 1, fp);
	fclose(fp);
}
