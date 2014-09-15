#define TAG_COUNT 32
#define OVECCOUNT 30
#define IDS_COUNT 1024
#define LOG_LINE 8092

sem_t g_sem;
pcre *list[IDS_COUNT];

struct IDS_XML_NODE {
        xmlChar *id;
        xmlChar *rule;
        xmlChar *description;
        xmlChar *impact;
        xmlChar *tag[TAG_COUNT];
};

struct IDS_XML_NODE *idsxml[IDS_COUNT];

void parse_node (xmlDocPtr doc, xmlNodePtr cur, struct IDS_XML_NODE *idsxml);
static void parseDoc(char *docname, struct IDS_XML_NODE **idsxml);
void *apache_parser(void *param);

