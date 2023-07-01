/*****************************************************************
 * File Name:        uuidlib.h
 * Description:
 * This header contains the prototype definitions
 *
 * Software distributed under the License is distributed on an
 * "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express
 * or implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
*****************************************************************/

#if defined __BORLANDC__ && defined __WIN32__
#ifndef EXPORT
    #define EXPORT _export
#endif
#else
#define EXPORT
#endif

#ifdef  __cplusplus
extern "C" {
#endif


extern char* fn_uuid_create(char* uuid);
extern char* fn_guid_create(char* guid);
extern char* fn_squash_guid(char* guid, char* uuid);
extern char* fn_expand_uuid(char* uuid, char* guid);
extern char* _uuid_to_guid(uuid_tt* _uuid, char* guid);
extern char* guid_to__uuid(char* guid, uuid_tt* _uuid);
extern char* _uuid_to_uuid(uuid_tt* _uuid, char* uuid);
extern uuid_tt* uuid_to__uuid(char* uuid, uuid_tt* _uuid);

#ifdef  __cplusplus
    }
#endif
