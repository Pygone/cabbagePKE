//
// Created by Pygone on 2023/12/11.
//

#include "symbol.h"
#include "string.h"

elf_symbol symbols[1024];
char sym_names[1024][32];
int sym_count = 0;

void quickSort(int l, int r)
{
	if (l >= r) return;
	int i = l, j = r;
	elf_symbol x = symbols[l];
	char temp_name[32];
	strcpy(temp_name, sym_names[l]);
	while (i < j) {
		while (i < j && symbols[j].st_value >= x.st_value) j--;
		if (i < j) {
			symbols[i] = symbols[j];
			strcpy(sym_names[i], sym_names[j]);
		}
		while (i < j && symbols[i].st_value < x.st_value) i++;
		if (i < j) {
			symbols[j] = symbols[i];
			strcpy(sym_names[j], sym_names[i]);
		}
	}
	symbols[i] = x;
	strcpy(sym_names[i], temp_name);
	quickSort(l, i - 1);
	quickSort(i + 1, r);
}