#include <string.h>
#include <stdio.h>
#include "sysdep.h"
#include "uuid.h"
#include "uuidlib.h"

int main(int argc, char **argv) {
	char *guid, *uuid;
	uuid_t	u;
	int i;

	guid = (char *) malloc(36);
	uuid = (char *) malloc(22);

	//strcpy(guid,"024db156-1dd2-11b2-85ed-931411acf515");
	strcpy(guid,"f65bd198-8dd1-11b2-85ed-931411acf515");
	printf("guid is   - %s\n",guid);

	fn_squash_guid(guid,uuid);
	printf("uuid is   - %s\n",uuid);

	fn_expand_uuid(uuid,guid);
	printf("guid id   - %s\n",guid);

	return 0;
}

void puid(uuid_t* u) {
      int i;

      printf("%8.8x-%4.4x-%4.4x-%2.2x%2.2x-", u->time_low, u->time_mid,
                 u->time_hi_and_version, u->clock_seq_hi_and_reserved,
					            u->clock_seq_low);
      for (i = 0; i < 6; i++)
                printf("%2.2x", u->node[i]);
      printf("\n");
};

