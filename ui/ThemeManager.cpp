#include <QApplication>
#include <QStyle>
#include <QStyleFactory>
#include <QProcess>
#include <QStandardPaths>
#include <memory>

#include "ThemeManager.hpp"

ThemeManager *themeManager = new ThemeManager;

extern QString ReadFileText(const QString &path);

void ThemeManager::InitializeSystemTheme() {
    if (initialized) return;
    initialized = true;
    system_style_name = qApp->style()->objectName();
    system_palette = qApp->palette();
    system_font = qApp->font();
    system_font.setPointSize(9);
    qApp->setFont(system_font);

#ifdef Q_OS_LINUX
    // Qt 5's GTK platform theme may provide a light palette even when GNOME
    // requests dark mode. Observe GNOME's preference without changing it.
    const QString settings = QStandardPaths::findExecutable("gsettings");
    if (!qEnvironmentVariable("XDG_CURRENT_DESKTOP").contains("GNOME", Qt::CaseInsensitive)
        || settings.isEmpty()) return;
    if (system_palette.color(QPalette::Window).lightness() < 128) {
        // Retain a light fallback when the application starts in native dark mode.
        const std::unique_ptr<QStyle> lightStyle(QStyleFactory::create("Fusion"));
        if (lightStyle) system_palette = lightStyle->standardPalette();
    }
    QProcess read;
    read.start(settings, {"get", "org.gnome.desktop.interface", "color-scheme"});
    if (read.waitForFinished(1000) && read.exitCode() == 0) {
        gnome_dark = read.readAllStandardOutput().trimmed() == "'prefer-dark'";
    }
    auto monitor = new QProcess(qApp);
    QObject::connect(monitor, &QProcess::readyReadStandardOutput, qApp, [this, monitor] {
        while (monitor->canReadLine()) {
            const QByteArray line = monitor->readLine().trimmed();
            if (!line.startsWith("color-scheme:")) continue;
            gnome_dark = line.contains("'prefer-dark'");
            if (current_theme == "0") ApplyTheme("0");
        }
    });
    monitor->start(settings, {"monitor", "org.gnome.desktop.interface", "color-scheme"});
    QObject::connect(qApp, &QCoreApplication::aboutToQuit, monitor, [monitor] {
        monitor->terminate();
        if (!monitor->waitForFinished(100)) {
            monitor->kill();
            monitor->waitForFinished(100);
        }
    });
#endif
}

void ThemeManager::ApplySystemPalette() {
    if (!gnome_dark) {
        qApp->setPalette(system_palette);
        QCoreApplication::setAttribute(Qt::AA_SetPalette, false);
        return;
    }
    QPalette dark;
    dark.setColor(QPalette::Window, QColor("#292929"));
    dark.setColor(QPalette::WindowText, QColor("#eeeeee"));
    dark.setColor(QPalette::Base, QColor("#202020"));
    dark.setColor(QPalette::AlternateBase, QColor("#303030"));
    dark.setColor(QPalette::Text, QColor("#eeeeee"));
    dark.setColor(QPalette::Button, QColor("#353535"));
    dark.setColor(QPalette::ButtonText, QColor("#eeeeee"));
    dark.setColor(QPalette::ToolTipBase, QColor("#353535"));
    dark.setColor(QPalette::ToolTipText, QColor("#eeeeee"));
    dark.setColor(QPalette::Highlight, QColor("#3584e4"));
    dark.setColor(QPalette::HighlightedText, Qt::white);
    dark.setColor(QPalette::Link, QColor("#78aeed"));
    dark.setColor(QPalette::LinkVisited, QColor("#c49cde"));
    dark.setColor(QPalette::BrightText, QColor("#ff8080"));
    dark.setColor(QPalette::Light, QColor("#555555"));
    dark.setColor(QPalette::Midlight, QColor("#454545"));
    dark.setColor(QPalette::Mid, QColor("#252525"));
    dark.setColor(QPalette::Dark, QColor("#181818"));
    dark.setColor(QPalette::Shadow, Qt::black);
    for (auto role : {QPalette::WindowText, QPalette::Text, QPalette::ButtonText}) {
        dark.setColor(QPalette::Disabled, role, QColor("#999999"));
    }
    qApp->setPalette(dark);
}

void ThemeManager::ApplyTheme(const QString &theme) {
    InitializeSystemTheme();

    bool isBuiltIn;
    const int themeId = theme.toInt(&isBuiltIn);
    QString qss;

    if (isBuiltIn && themeId != 0) {
        QString path;
        switch (themeId) {
            case 1:
                path = ":/themes/feiyangqingyun/qss/flatgray.css";
                break;
            case 2:
                path = ":/themes/feiyangqingyun/qss/lightblue.css";
                break;
            case 3:
                path = ":/themes/feiyangqingyun/qss/blacksoft.css";
                break;
            default:
                return;
        }
        qss = ReadFileText(path);
        qss.replace(":/qss/", ":/themes/feiyangqingyun/qss/");

        const QString paletteColor = qss.mid(20, 7);
        qApp->setStyleSheet("");
        qApp->setPalette(QPalette(paletteColor));
        qApp->setStyleSheet(qss + "\n" + ReadFileText(":/neko/neko.css"));
    } else {
        const QString styleName = isBuiltIn ? (gnome_dark ? QStringLiteral("Fusion") : system_style_name) : theme;
        QStyle *style = nullptr;
        if (theme != current_theme || qApp->style()->objectName() != styleName) {
            style = QStyleFactory::create(styleName);
            if (!style) {
                return;
            }
        }

        qApp->setStyleSheet("");
        if (style) {
            qApp->setStyle(style);
        }

        if (isBuiltIn) {
            ApplySystemPalette();
        } else {
            qApp->setPalette(qApp->style()->standardPalette());
        }
        qApp->setStyleSheet(ReadFileText(":/neko/neko.css"));
    }

    current_theme = theme;
    qApp->setFont(system_font);
}
