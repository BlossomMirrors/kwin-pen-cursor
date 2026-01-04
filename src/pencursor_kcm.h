/*
    SPDX-FileCopyrightText: 2025 Blossom
    SPDX-License-Identifier: MIT
*/

#ifndef PENCURSOR_KCM_H
#define PENCURSOR_KCM_H

#include <KCModule>

class KUrlRequester;

class PenCursorEffectConfig : public KCModule
{
    Q_OBJECT

public:
    explicit PenCursorEffectConfig(QObject *parent, const KPluginMetaData &data);

public Q_SLOTS:
    void save() override;
    void defaults() override;

private:
    KUrlRequester *m_urlRequester;
};

#endif
