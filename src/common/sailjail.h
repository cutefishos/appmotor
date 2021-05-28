#ifndef SAILJAIL_H_
# define SAILJAIL_H_

# include <stdbool.h>
# include <glib.h>

G_BEGIN_DECLS

bool sailjail_verify_launch(const char *desktop, const char **argv);

G_END_DECLS

#endif // SAILJAIL_H_
