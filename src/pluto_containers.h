ARRAY_DEF (arr_entity, ecs_entity_t, M_BASIC_OPLIST)
ARRAY_DEF (mat2d_entity, arr_entity_t, M_ARRAY_OPLIST (arr_entity))
ARRAY_DEF (mat3d_entity, mat2d_entity_t, M_ARRAY_OPLIST (mat2d_entity))

DICT_DEF2 (dict_string_to_query_ptr, string_t, M_STRING_OPLIST, ecs_query_t *,
           M_PTR_OPLIST)