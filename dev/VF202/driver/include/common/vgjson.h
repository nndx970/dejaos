/*
  Copyright (c) 2009-2017 Dave Gamble and vJSON contributors

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef __VBAR_M_VGJSON__
#define __VBAR_M_VGJSON__

#include <vbar/export.h>

#ifdef __cplusplus
extern "C"
{
#endif


/* project version */
#define VJSON_VERSION_MAJOR 1
#define VJSON_VERSION_MINOR 7
#define VJSON_VERSION_PATCH 15

#include <stddef.h>

/* vJSON Types: */
#define vJSON_Invalid (0)
#define vJSON_False  (1 << 0)
#define vJSON_True   (1 << 1)
#define vJSON_NULL   (1 << 2)
#define vJSON_Number (1 << 3)
#define vJSON_String (1 << 4)
#define vJSON_Array  (1 << 5)
#define vJSON_Object (1 << 6)
#define vJSON_Raw    (1 << 7) /* raw json */

#define vJSON_IsReference 256
#define vJSON_StringIsConst 512

/* The vJSON structure: */
typedef struct vJSON
{
    /* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
    struct vJSON *next;
    struct vJSON *prev;
    /* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */
    struct vJSON *child;

    /* The type of the item, as above. */
    int type;

    /* The item's string, if type==vJSON_String  and type == vJSON_Raw */
    char *valuestring;
    /* writing to valueint is DEPRECATED, use vJSON_SetNumberValue instead */
    int valueint;
    /* The item's number, if type==vJSON_Number */
    double valuedouble;

    /* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
    char *string;
} vJSON;

typedef struct vJSON_Hooks
{
      /* malloc/free are CDECL on Windows regardless of the default calling convention of the compiler, so ensure the hooks allow passing those functions directly. */
      void *(*malloc_fn)(size_t sz);
      void (*free_fn)(void *ptr);
} vJSON_Hooks;

typedef int vJSON_bool;

/* Limits how deeply nested arrays/objects can be before vJSON rejects to parse them.
 * This is to prevent stack overflows. */
#ifndef CJSON_NESTING_LIMIT
#define CJSON_NESTING_LIMIT 1000
#endif

/* returns the version of vJSON as a string */
VBAR_EXPORT_API const char *vJSON_Version(void);

/* Supply malloc, realloc and free functions to vJSON */
VBAR_EXPORT_API void vJSON_InitHooks(vJSON_Hooks* hooks);

/* Memory Management: the caller is always responsible to free the results from all variants of vJSON_Parse (with vJSON_Delete) and vJSON_Print (with stdlib free, vJSON_Hooks.free_fn, or vJSON_free as appropriate). The exception is vJSON_PrintPreallocated, where the caller has full responsibility of the buffer. */
/* Supply a block of JSON, and this returns a vJSON object you can interrogate. */
VBAR_EXPORT_API vJSON *vJSON_Parse(const char *value);
VBAR_EXPORT_API vJSON *vJSON_ParseWithLength(const char *value, size_t buffer_length);
/* ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to retrieve the pointer to the final byte parsed. */
/* If you supply a ptr in return_parse_end and parsing fails, then return_parse_end will contain a pointer to the error so will match vJSON_GetErrorPtr(). */
VBAR_EXPORT_API vJSON *vJSON_ParseWithOpts(const char *value, const char **return_parse_end, vJSON_bool require_null_terminated);
VBAR_EXPORT_API vJSON *vJSON_ParseWithLengthOpts(const char *value, size_t buffer_length, const char **return_parse_end, vJSON_bool require_null_terminated);

/* Render a vJSON entity to text for transfer/storage. */
VBAR_EXPORT_API char *vJSON_Print(const vJSON *item);
/* Render a vJSON entity to text for transfer/storage without any formatting. */
VBAR_EXPORT_API char *vJSON_PrintUnformatted(const vJSON *item);
/* Render a vJSON entity to text using a buffered strategy. prebuffer is a guess at the final size. guessing well reduces reallocation. fmt=0 gives unformatted, =1 gives formatted */
VBAR_EXPORT_API char *vJSON_PrintBuffered(const vJSON *item, int prebuffer, vJSON_bool fmt);
/* Render a vJSON entity to text using a buffer already allocated in memory with given length. Returns 1 on success and 0 on failure. */
/* NOTE: vJSON is not always 100% accurate in estimating how much memory it will use, so to be safe allocate 5 bytes more than you actually need */
VBAR_EXPORT_API vJSON_bool vJSON_PrintPreallocated(vJSON *item, char *buffer, const int length, const vJSON_bool format);
/* Delete a vJSON entity and all subentities. */
VBAR_EXPORT_API void vJSON_Delete(vJSON *item);

/* Returns the number of items in an array (or object). */
VBAR_EXPORT_API int vJSON_GetArraySize(const vJSON *array);
/* Retrieve item number "index" from array "array". Returns NULL if unsuccessful. */
VBAR_EXPORT_API vJSON *vJSON_GetArrayItem(const vJSON *array, int index);
/* Get item "string" from object. Case insensitive. */
VBAR_EXPORT_API vJSON *vJSON_GetObjectItem(const vJSON * const object, const char * const string);
VBAR_EXPORT_API vJSON *vJSON_GetObjectItemCaseSensitive(const vJSON * const object, const char * const string);
VBAR_EXPORT_API vJSON_bool vJSON_HasObjectItem(const vJSON *object, const char *string);
/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when vJSON_Parse() returns 0. 0 when vJSON_Parse() succeeds. */
VBAR_EXPORT_API const char *vJSON_GetErrorPtr(void);

/* Check item type and return its value */
VBAR_EXPORT_API char *vJSON_GetStringValue(const vJSON * const item);
VBAR_EXPORT_API double vJSON_GetNumberValue(const vJSON * const item);

/* These functions check the type of an item */
VBAR_EXPORT_API vJSON_bool vJSON_IsInvalid(const vJSON * const item);
VBAR_EXPORT_API vJSON_bool vJSON_IsFalse(const vJSON * const item);
VBAR_EXPORT_API vJSON_bool vJSON_IsTrue(const vJSON * const item);
VBAR_EXPORT_API vJSON_bool vJSON_IsBool(const vJSON * const item);
VBAR_EXPORT_API vJSON_bool vJSON_IsNull(const vJSON * const item);
VBAR_EXPORT_API vJSON_bool vJSON_IsNumber(const vJSON * const item);
VBAR_EXPORT_API vJSON_bool vJSON_IsString(const vJSON * const item);
VBAR_EXPORT_API vJSON_bool vJSON_IsArray(const vJSON * const item);
VBAR_EXPORT_API vJSON_bool vJSON_IsObject(const vJSON * const item);
VBAR_EXPORT_API vJSON_bool vJSON_IsRaw(const vJSON * const item);

/* These calls create a vJSON item of the appropriate type. */
VBAR_EXPORT_API vJSON *vJSON_CreateNull(void);
VBAR_EXPORT_API vJSON *vJSON_CreateTrue(void);
VBAR_EXPORT_API vJSON *vJSON_CreateFalse(void);
VBAR_EXPORT_API vJSON *vJSON_CreateBool(vJSON_bool boolean);
VBAR_EXPORT_API vJSON *vJSON_CreateNumber(double num);
VBAR_EXPORT_API vJSON *vJSON_CreateString(const char *string);
/* raw json */
VBAR_EXPORT_API vJSON *vJSON_CreateRaw(const char *raw);
VBAR_EXPORT_API vJSON *vJSON_CreateArray(void);
VBAR_EXPORT_API vJSON *vJSON_CreateObject(void);

/* Create a string where valuestring references a string so
 * it will not be freed by vJSON_Delete */
VBAR_EXPORT_API vJSON *vJSON_CreateStringReference(const char *string);
/* Create an object/array that only references it's elements so
 * they will not be freed by vJSON_Delete */
VBAR_EXPORT_API vJSON *vJSON_CreateObjectReference(const vJSON *child);
VBAR_EXPORT_API vJSON *vJSON_CreateArrayReference(const vJSON *child);

/* These utilities create an Array of count items.
 * The parameter count cannot be greater than the number of elements in the number array, otherwise array access will be out of bounds.*/
VBAR_EXPORT_API vJSON *vJSON_CreateIntArray(const int *numbers, int count);
VBAR_EXPORT_API vJSON *vJSON_CreateFloatArray(const float *numbers, int count);
VBAR_EXPORT_API vJSON *vJSON_CreateDoubleArray(const double *numbers, int count);
VBAR_EXPORT_API vJSON *vJSON_CreateStringArray(const char *const *strings, int count);

/* Append item to the specified array/object. */
VBAR_EXPORT_API vJSON_bool vJSON_AddItemToArray(vJSON *array, vJSON *item);
VBAR_EXPORT_API vJSON_bool vJSON_AddItemToObject(vJSON *object, const char *string, vJSON *item);
/* Use this when string is definitely const (i.e. a literal, or as good as), and will definitely survive the vJSON object.
 * WARNING: When this function was used, make sure to always check that (item->type & vJSON_StringIsConst) is zero before
 * writing to `item->string` */
VBAR_EXPORT_API vJSON_bool vJSON_AddItemToObjectCS(vJSON *object, const char *string, vJSON *item);
/* Append reference to item to the specified array/object. Use this when you want to add an existing vJSON to a new vJSON, but don't want to corrupt your existing vJSON. */
VBAR_EXPORT_API vJSON_bool vJSON_AddItemReferenceToArray(vJSON *array, vJSON *item);
VBAR_EXPORT_API vJSON_bool vJSON_AddItemReferenceToObject(vJSON *object, const char *string, vJSON *item);

/* Remove/Detach items from Arrays/Objects. */
VBAR_EXPORT_API vJSON *vJSON_DetachItemViaPointer(vJSON *parent, vJSON * const item);
VBAR_EXPORT_API vJSON *vJSON_DetachItemFromArray(vJSON *array, int which);
VBAR_EXPORT_API void vJSON_DeleteItemFromArray(vJSON *array, int which);
VBAR_EXPORT_API vJSON *vJSON_DetachItemFromObject(vJSON *object, const char *string);
VBAR_EXPORT_API vJSON *vJSON_DetachItemFromObjectCaseSensitive(vJSON *object, const char *string);
VBAR_EXPORT_API void vJSON_DeleteItemFromObject(vJSON *object, const char *string);
VBAR_EXPORT_API void vJSON_DeleteItemFromObjectCaseSensitive(vJSON *object, const char *string);

/* Update array items. */
VBAR_EXPORT_API vJSON_bool vJSON_InsertItemInArray(vJSON *array, int which, vJSON *newitem); /* Shifts pre-existing items to the right. */
VBAR_EXPORT_API vJSON_bool vJSON_ReplaceItemViaPointer(vJSON * const parent, vJSON * const item, vJSON * replacement);
VBAR_EXPORT_API vJSON_bool vJSON_ReplaceItemInArray(vJSON *array, int which, vJSON *newitem);
VBAR_EXPORT_API vJSON_bool vJSON_ReplaceItemInObject(vJSON *object,const char *string,vJSON *newitem);
VBAR_EXPORT_API vJSON_bool vJSON_ReplaceItemInObjectCaseSensitive(vJSON *object,const char *string,vJSON *newitem);

/* Duplicate a vJSON item */
VBAR_EXPORT_API vJSON *vJSON_Duplicate(const vJSON *item, vJSON_bool recurse);
/* Duplicate will create a new, identical vJSON item to the one you pass, in new memory that will
 * need to be released. With recurse!=0, it will duplicate any children connected to the item.
 * The item->next and ->prev pointers are always zero on return from Duplicate. */
/* Recursively compare two vJSON items for equality. If either a or b is NULL or invalid, they will be considered unequal.
 * case_sensitive determines if object keys are treated case sensitive (1) or case insensitive (0) */
VBAR_EXPORT_API vJSON_bool vJSON_Compare(const vJSON * const a, const vJSON * const b, const vJSON_bool case_sensitive);

/* Minify a strings, remove blank characters(such as ' ', '\t', '\r', '\n') from strings.
 * The input pointer json cannot point to a read-only address area, such as a string constant,
 * but should point to a readable and writable address area. */
VBAR_EXPORT_API void vJSON_Minify(char *json);

/* Helper functions for creating and adding items to an object at the same time.
 * They return the added item or NULL on failure. */
VBAR_EXPORT_API vJSON *vJSON_AddNullToObject(vJSON * const object, const char * const name);
VBAR_EXPORT_API vJSON *vJSON_AddTrueToObject(vJSON * const object, const char * const name);
VBAR_EXPORT_API vJSON *vJSON_AddFalseToObject(vJSON * const object, const char * const name);
VBAR_EXPORT_API vJSON *vJSON_AddBoolToObject(vJSON * const object, const char * const name, const vJSON_bool boolean);
VBAR_EXPORT_API vJSON *vJSON_AddNumberToObject(vJSON * const object, const char * const name, const double number);
VBAR_EXPORT_API vJSON *vJSON_AddStringToObject(vJSON * const object, const char * const name, const char * const string);
VBAR_EXPORT_API vJSON *vJSON_AddRawToObject(vJSON * const object, const char * const name, const char * const raw);
VBAR_EXPORT_API vJSON *vJSON_AddObjectToObject(vJSON * const object, const char * const name);
VBAR_EXPORT_API vJSON *vJSON_AddArrayToObject(vJSON * const object, const char * const name);

/* When assigning an integer value, it needs to be propagated to valuedouble too. */
#define vJSON_SetIntValue(object, number) ((object) ? (object)->valueint = (object)->valuedouble = (number) : (number))
/* helper for the vJSON_SetNumberValue macro */
VBAR_EXPORT_API double vJSON_SetNumberHelper(vJSON *object, double number);
#define vJSON_SetNumberValue(object, number) ((object != NULL) ? vJSON_SetNumberHelper(object, (double)number) : (number))
/* Change the valuestring of a vJSON_String object, only takes effect when type of object is vJSON_String */
VBAR_EXPORT_API char *vJSON_SetValuestring(vJSON *object, const char *valuestring);

/* Macro for iterating over an array or object */
#define vJSON_ArrayForEach(element, array) for(element = (array != NULL) ? (array)->child : NULL; element != NULL; element = element->next)

/* malloc/free objects using the malloc/free functions that have been set with vJSON_InitHooks */
VBAR_EXPORT_API void *vJSON_malloc(size_t size);
VBAR_EXPORT_API void vJSON_free(void *object);


//#define VJSON_TABLE_DUMP(table)
#ifndef VJSON_TABLE_DUMP
#define VJSON_TABLE_DUMP(table) do {\
        vJSON *element = NULL;\
        if ((table)) {\
            vJSON_ArrayForEach(element, (table)) { \
                if (vJSON_IsNumber(element)) {\
                    fprintf(stderr, "%d|%s()|info:>>>>>>: %s = %d (%f)\n",__LINE__, __func__,\
                        element->string, element->valueint, element->valuedouble); \
                } else if (vJSON_IsString((element))) {\
                    fprintf(stderr, "%d|%s()|info:>>>>>>: %s = \"%s\"\n",__LINE__, __func__,\
                        element->string, element->valuestring); \
                }\
            }\
        }\
    } while (0)
#endif

#define vjsonSusCode (0)
#define vjsonErrCode (-((int)(~0U>>1)) - 1)

/* item 为类型 */
#define vjson_obj_get_string(obj, field, str) do {vJSON *item = NULL; item = vJSON_GetObjectItem((obj), (field)); \
                                                   (str) = (item?vJSON_GetStringValue(item):NULL);}while(0)
#define vjson_obj_get_num(obj, field, ret, num) do {vJSON *item = NULL; item = vJSON_GetObjectItem((obj), (field));ret=vjsonSusCode; \
                                                   if (!item||!(vJSON_IsNumber(item))){(ret)=vjsonErrCode;break;} \
                                                   (num) = item->valuedouble;}while(0)
#define vjson_obj_get_bool(obj, field, ret ,val) do {vJSON *item = NULL; item = vJSON_GetObjectItem((obj), (field));ret=vjsonSusCode; \
                                                    if (!item||!(vJSON_IsBool(item))){(ret)=vjsonErrCode;break;} \
                                                    (val) = ((vJSON_IsTrue(item))?true:false);}while(0)
#define vjson_obj_get_subobj(obj, field, subobj) do {(subobj) = vJSON_GetObjectItem((obj), (field));}while(0)
/*add value to json obj*/
#define vjson_obj_add_item(obj, name, item)  do{if (obj) vJSON_AddItemToObject((obj), (name), (item));}while(0)
#define vjson_obj_add_string(obj, name, str)  do{if (obj) vJSON_AddItemToObject((obj), (name), vJSON_CreateString((str)));}while(0)
#define vjson_obj_add_num(obj, name, num)  do{if (obj) vJSON_AddItemToObject((obj), (name), vJSON_CreateNumber((num)));}while(0)
#define vjson_obj_add_bool(obj, name, num)  do{if (obj) vJSON_AddItemToObject((obj), (name), vJSON_CreateBool((num)?1:0));}while(0)

#define vjson_arry_add_item(array,   item)  do{if (array) vJSON_AddItemToArray((array), (item));}while(0)
#define vjson_arry2string(root, string)  do{if ((root)) (string) = vJSON_PrintUnformatted((root));}while(0)
#define vjson_string2arry(string, root)  do{if ((string)) (root) = vJSON_Parse((string));}while(0)
#define vjson_string2arry_withlen(string, stringlen, root)  do{if ((string)) (root) = vJSON_ParseWithLength((string), (stringlen));}while(0)
#define vjson_arry_free(root)  do{if ((root)) vJSON_Delete((root));}while(0)


#ifdef __cplusplus
}
#endif

#endif
