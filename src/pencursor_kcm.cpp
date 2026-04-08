/*
    SPDX-FileCopyrightText: 2025 Blossom
    SPDX-License-Identifier: MIT
*/

#include "pencursor_kcm.h"
#include "kwineffects_interface.h"
#include <KPluginFactory>
#include <KConfigGroup>
#include <KSharedConfig>
#include <KFile>
#include <KLocalizedString>
#include <KUrlRequester>
#include <QFormLayout>
#include <QUrl>

K_PLUGIN_CLASS(PenCursorEffectConfig)

PenCursorEffectConfig::PenCursorEffectConfig(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    auto *layout = new QFormLayout(widget());
    
    m_urlRequester = new KUrlRequester(widget());
    m_urlRequester->setNameFilters({QStringLiteral("*.svg")});
    m_urlRequester->setMode(KFile::File | KFile::ExistingOnly);
    
    layout->addRow(i18nd("kwin_pencursor", "Cursor SVG:"), m_urlRequester);
    
    auto config = KSharedConfig::openConfig(QStringLiteral("pencursorrc"));
    auto group = config->group(QStringLiteral("General"));
    QString cursorPath = group.readEntry("CursorPath", "/usr/share/kwin/effects/pen-point-cursor.svg");
    m_urlRequester->setUrl(QUrl::fromLocalFile(cursorPath));
    
    connect(m_urlRequester, &KUrlRequester::textChanged, this, [this]() {
        setNeedsSave(true);
    });
}

void PenCursorEffectConfig::save()
{
    auto config = KSharedConfig::openConfig(QStringLiteral("pencursorrc"));
    auto group = config->group(QStringLiteral("General"));
    group.writeEntry("CursorPath", m_urlRequester->url().toLocalFile());
    config->sync();
    
    KCModule::save();
    
    OrgKdeKwinEffectsInterface interface(QStringLiteral("org.kde.KWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("pencursor"));
}

void PenCursorEffectConfig::defaults()
{
    m_urlRequester->setUrl(QUrl::fromLocalFile(QStringLiteral("/usr/share/kwin/effects/pen-point-cursor.svg")));
    setNeedsSave(true);
    KCModule::defaults();
}

#include "pencursor_kcm.moc"
#include "moc_pencursor_kcm.cpp"
