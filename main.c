#include "main.h"
void print_type(int t) {
	const char* types[] = {
	  "json_type_string",
	  "json_type_number",
	  "json_type_object",
	  "json_type_array",
	  "json_type_true",
	  "json_type_false",
	  "json_type_null"
	};
	printf("%s\n",types[t]);
}

// INDEX FROM 0
struct json_object_s* get_periodic_element_object(struct json_array_s* table, size_t index) {
    struct json_array_element_s* element = table->start;
    for (size_t i=0; i<index; ++i) {
        element=element->next;
    }
    struct json_value_s* element_val = element->value;
    struct json_object_s* element_obj = element_val->payload;
    return element_obj;
}

struct json_value_s* index_object_element(struct json_object_s* element, size_t index) {
    struct json_object_element_s* indexed_elem = element->start;
    for (size_t i = 0; i < index; ++i) {
        indexed_elem = indexed_elem->next;
    }
    return indexed_elem->value;
}
struct json_string_s* get_element_string(struct json_object_s* element, size_t index) {
    // TODO: Return string from elment index
    struct json_value_s* string = index_object_element(element, index);
    if (string->type != json_type_string) {
        perror("Indexed Element is not JSON String!\n");
        return NULL;
    } else {
        return (struct json_string_s*) string->payload;
    }
}
struct json_string_s* get_element_tag(struct json_object_s* element, size_t index) {
    // TODO: Return string from elment index
    struct json_object_element_s* indexed_elem = element->start;
    for (size_t i = 0; i < index; ++i) {
        indexed_elem = indexed_elem->next;
    }
    return (struct json_string_s*) indexed_elem->name;
}
struct json_number_s* get_element_number(struct json_object_s* element, size_t index) {
    // TODO: Return string from elment index
    struct json_value_s* number = index_object_element(element, index);
    if (number->type != json_type_number) {
        perror("Indexed Element is not JSON Number!\n");
        return NULL;
    } else {
        return (struct json_number_s*) number->payload;
    }
}
const char TABLE_DIR[] = "./PeriodicTableJSON.json";
int main(int argc, char **argv) {
    /***********************************************/
    /***********OPEN THE FILE AND STUFF...**********/
	FILE *json = fopen(TABLE_DIR, "r");
	if (json == NULL) {
		printf("Failed to locate JSON file!\n");
		exit(EXIT_FAILURE);
	}
	char* buffer = NULL;
	size_t len;
	ssize_t bytes_read = getdelim(&buffer, &len, '\0', json);
	if (bytes_read != -1) {
		printf("File operation successfull!\n");
		printf("Length of buffer: %lu\n", len);
	} else {
		printf("Failed to load JSON file!\n");
		exit(EXIT_FAILURE);
	}
    /***********************************************/
	struct json_value_s* root = json_parse(buffer, strlen(buffer));	
	assert(root->type == json_type_object);
	struct json_object_s* object = (struct json_object_s*)root->payload;
	struct json_object_element_s* a = object->start;
	assert(a->value->type == json_type_array);
	struct json_array_s* elements = (struct json_array_s*) a->value->payload;
	printf("%lu\n",elements->length);
    te_variable element_variables[121];
    double *atomic_masses = malloc(121 * sizeof *atomic_masses);
    for (size_t i = 0; i < elements->length; ++i) {
        struct json_object_s* element = get_periodic_element_object(elements, i);
        struct json_string_s* element_tag = get_element_tag(element,19);
        /* 2 next for amu */
        /* 19 next for symbol */
        struct json_string_s* element_name = get_element_string(element, 19);
        struct json_number_s* amu = get_element_number(element, 2);
        printf("%s : %s\n", element_tag->string, element_name->string);
        element_tag = get_element_tag(element,2);
        printf("%s : %s\n", element_tag->string, amu->number);
        double amu2 = atof(amu->number);
        atomic_masses[i] = amu2;
        element_variables[i] = (te_variable){element_name->string, atomic_masses+i, TE_VARIABLE};
    }
    double avogadro = 6.02214076E23;
    double ideal_gas = 8.3144598e0;
    element_variables[119] = (te_variable){"avogadro", &avogadro, TE_VARIABLE};
    element_variables[120] = (te_variable){"r", &ideal_gas, TE_VARIABLE};

    if (argc == 3 && strcmp(argv[1], "-e") == 0) {
        if (eval(argv[2], element_variables) == -1) {
            return 1;
        } else {
            return 0;
        }
    } else if (argc == 1) {
        repl(element_variables);
        return 0;
    } else {
        printf("Usage: %s\n", argv[0]);
        printf("       %s -e <expression>\n", argv[0]);
        return 1;
    }
    free(atomic_masses);
	fclose(json);
	exit(EXIT_SUCCESS);
}
