     #include "copyrt.h"
//     #undef uuid_t
	 
     typedef struct _uuid_t {
         unsigned32          time_low;
         unsigned16          time_mid;
         unsigned16          time_hi_and_version;
         unsigned8           clock_seq_hi_and_reserved;
         unsigned8           clock_seq_low;
         byte                node[6];
     } uuid_tt;

     /* uuid_create -- generate a UUID */
     int uuid_create(uuid_tt * uuid);

     /* uuid_create_from_name -- create a UUID using a "name"
        from a "name space" */
     void uuid_create_from_name(
       uuid_tt * uuid,        /* resulting UUID */
       uuid_tt nsid,          /* UUID to serve as context, so identical
                                names from different name spaces generate
                                different UUIDs */
       void * name,          /* the name from which to generate a UUID */
       int namelen           /* the length of the name */
     );

     /* uuid_compare --  Compare two UUID's "lexically" and return
             -1   u1 is lexically before u2
              0   u1 is equal to u2
              1   u1 is lexically after u2
        Note:   lexical ordering is not temporal ordering!
     */
     int uuid_ttcompare(uuid_tt *u1, uuid_tt *u2);
