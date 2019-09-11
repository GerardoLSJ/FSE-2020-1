#include <stdio.h>

int main(int argc, char **argv) {
    int i,j;
    
    j = 31;
	for(i = 1 ; i <= 15 ; i++){
		printf("\x1b[%dm#%d: FSE2020-1 Fernanda Ortiz & Abril Robles \n",j,i);
		if(j == 37){
			j = 31;
	    }
		else{
			j++;
		}
	}

	return 0;
}
