#include <sql_class.h>
#include <table.h>
#include <sql_show.h>
#include <sql_parse.h>

#define I_S_SCRIPT "/tmp/test.pl 2>&1"

static struct st_mysql_information_schema i_s_script_table_info=
{
  MYSQL_INFORMATION_SCHEMA_INTERFACE_VERSION
};

static ST_FIELD_INFO i_s_script_fields[]=
{
  { "name",   255, MYSQL_TYPE_STRING, 0, 0, 0, 0},
  {"value", 65535, MYSQL_TYPE_STRING, 0, 0, 0, 0}
};

static int scripting_i_s (THD *thd, TABLE_LIST *tables, Item *cond)
{
  if (check_global_access(thd, SUPER_ACL))
    return 0;

  TABLE *table= tables->table;
  FILE *pipe= popen(I_S_SCRIPT, "r");
  char name_col[255]= "";
  char value_col[65535]= "";
  char buff[65535]= "";

  while (fgets(buff, 65535, pipe))
  {
    sscanf(buff, "%s %s", name_col, value_col);
    table->field[0]->store(name_col, strlen(name_col), system_charset_info);
    table->field[1]->store(value_col, strlen(value_col), system_charset_info);
    table->file->ha_write_row(table->record[0]);
  }
  pclose(pipe);
  return 0;
}

static int i_s_script_init (void *ptr)
{
  ST_SCHEMA_TABLE *schema_table= (ST_SCHEMA_TABLE*) ptr;

  schema_table->fields_info= i_s_script_fields;
  schema_table->fill_table= scripting_i_s;
  return 0;
}

mysql_declare_plugin (scripting)
{
  MYSQL_INFORMATION_SCHEMA_PLUGIN,
  &i_s_script_table_info,
  "scripting_i_s",
  "yoku0825",
  "scripting information_schema plugin",
  PLUGIN_LICENSE_GPL,
  i_s_script_init,
  NULL,
  0x0001,
  NULL,
  NULL,
  NULL,
  0
}
mysql_declare_plugin_end;
