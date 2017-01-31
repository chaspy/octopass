#include "nss_octopass-passwd.c"
#include "nss_octopass-group.c"
#include "nss_octopass-shadow.c"
#include "nss_octopass.c"

void show_pwent(struct passwd *pwent)
{
  printf("%s:%s:%ld:%ld:%s:%s:%s\n", pwent->pw_name, pwent->pw_passwd, pwent->pw_uid, pwent->pw_gid, pwent->pw_gecos,
         pwent->pw_dir, pwent->pw_shell);
}

void show_grent(struct group *grent)
{
  printf("%s:%s:%ld", grent->gr_name, grent->gr_passwd, grent->gr_gid);
  const int num = sizeof grent->gr_mem / sizeof grent->gr_mem[0];
  int i;

  for (i = 0; i < num; i++) {
    printf(":%s", grent->gr_mem[i]);
  }

  if (num == 0) {
    printf(":\n");
  } else {
    printf("\n");
  }
}

void exec_team_id(void)
{
  struct config con;
  nss_octopass_config_loading(&con, NSS_OCTOPASS_CONFIG_FILE);
  int id = nss_octopass_team_id(&con);
  printf("endpoint: %s, organization: %s, team: %s, team_id: %d\n", con.endpoint, con.organization, con.team, id);
}

void exec_getpwnam_r(const char *name)
{
  enum nss_status status;
  struct passwd pwent;
  int err    = 0;
  int buflen = 2048;
  char buf[buflen];
  status = _nss_octopass_getpwnam_r(name, &pwent, buf, buflen, &err);
  show_pwent(&pwent);
}

void exec_getpwuid_r(uid_t uid)
{
  enum nss_status status;
  struct passwd pwent;
  int err    = 0;
  int buflen = 2048;
  char buf[buflen];
  status = _nss_octopass_getpwuid_r(uid, &pwent, buf, buflen, &err);
  show_pwent(&pwent);
}

void exec_pwlist(void)
{
  enum nss_status status;
  struct passwd pwent;
  int err                    = 0;
  unsigned long entry_number = 0;
  int buflen                 = 2048;
  char buf[buflen];

  status = _nss_octopass_setpwent(0);

  status = 0;
  while (!status) {
    entry_number += 1;
    err    = 0;
    status = _nss_octopass_getpwent_r(&pwent, buf, buflen, &err);
    show_pwent(&pwent);
  }

  status = _nss_octopass_endpwent();
}

void exec_getgrnam_r(const char *name)
{
}

void exec_getgrgid_r(gid_t gid)
{
}

void exec_grlist(void)
{
  enum nss_status status;
  struct group grent;
  int err                    = 0;
  unsigned long entry_number = 0;
  int buflen                 = 2048;
  char buf[buflen];

  status = _nss_octopass_setgrent(0);

  status = 0;
  while (!status) {
    entry_number += 1;
    err    = 0;
    status = _nss_octopass_getgrent_r(&grent, buf, buflen, &err);
    show_grent(&grent);
  }

  status = _nss_octopass_endgrent();
}

void help(void)
{
  printf("Usage: nss-octopass [CMD]\n");
  printf("\n");
  printf("Command:\n");
  printf("  passwd             get passwds, call setpwent(3), getpwent(3), endpwent(3)\n");
  printf("  getpwnam [NAME]    get a passwd by name, call getpwnam(3)\n");
  printf("  getpwuid [UID]     get a passwd by uid, call getgpwuid(3)\n");
  printf("  group              get groups, call setgrent(3), getgrent(3), endgrent(3)\n");
  printf("  getgrnam [NAME]    get a group by name, call getgrnam(3)\n");
  printf("  getgrgid [GID]     get a group by gid, call getgpwuid(3)\n");
  printf("  shadow             get shadows, call setspent(3), getspent(3), endspent(3)\n");
  printf("  getspnam [NAME]    get a shadow by name, call getspnam(3)\n");
  printf("\n");
}

int main(int argc, char **argv)
{
  if (argc < 2 || !strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")) {
    help();
    return 2;
  }

  if (strcmp(argv[1], "passwd") == 0) {
    exec_pwlist();
    return 0;
  }

  if (strcmp(argv[1], "getpwnam") == 0) {
    if (argc < 3) {
      fprintf(stderr, "getpwunam requires a username\n");
      return 1;
    }
    const char *name = (char *)argv[2];
    exec_getpwnam_r(name);
    return 0;
  }

  if (strcmp(argv[1], "getpwuid") == 0) {
    if (argc < 3) {
      fprintf(stderr, "getpwuid requires a user id\n");
      return 1;
    }
    uid_t uid = (uid_t)atol(argv[2]);
    exec_getpwuid_r(uid);
    return 0;
  }

  if (strcmp(argv[1], "group") == 0) {
    exec_grlist();
    return 0;
  }

  if (strcmp(argv[1], "getgrnam") == 0) {
    if (argc < 3) {
      fprintf(stderr, "getgrunam requires a group name\n");
      return 1;
    }
    const char *name = (char *)argv[2];
    exec_getgrnam_r(name);
    return 0;
  }

  if (strcmp(argv[1], "getgrgid") == 0) {
    if (argc < 3) {
      fprintf(stderr, "getgrgid requires a group id\n");
      return 1;
    }
    gid_t gid = (gid_t)atol(argv[2]);
    exec_getgrgid_r(gid);
    return 0;
  }

  fprintf(stderr, "Invalid command: %s\n", argv[1]);
  return 1;
}
