
#ifndef DATABASE_ID_MGR_H
#define DATABASE_ID_MGR_H

void set_id(int page_id);
int get_id();
bool has_id(int id);
bool release_id(int id);
bool empty_id();

#endif //DATABASE_ID_MGR_H
