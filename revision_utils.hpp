#ifndef REVISION_UTILS_HPP__
#define REVISION_UTILS_HPP__

#include <QString>

QString revision (QString const& svn_rev_string = QString {});
QString version (bool include_patch = true);
QString program_title (QString const& revision = QString {});

// This fork's own version, which is not the base project's. Defined in one
// place because the title bar, the beta log and the installer all show it.
QString wsj_tay_version ();

#endif
