#pragma once

#include <QFont>
#include <QPalette>
#include <QString>

class ThemeManager {
public:
    QString system_style_name = "";
    QString current_theme = "0"; // int: 0:system 1+:builtin string: QStyleFactory

    void ApplyTheme(const QString &theme);

private:
    void InitializeSystemTheme();
    void ApplySystemPalette();
    QPalette system_palette;
    QFont system_font;
    bool initialized = false;
    bool gnome_dark = false;
};

extern ThemeManager *themeManager;
