/*
    SPDX-FileCopyrightText: 2025 Blossom
    SPDX-License-Identifier: MIT
*/

#ifndef MAIN_H
#define MAIN_H

#include <effect/effect.h>
#include <memory>

namespace KWin
{

class PenInputListener;
class OverlayRenderer;

class PenCursorEffect : public Effect
{
    Q_OBJECT
    
public:
    PenCursorEffect();
    ~PenCursorEffect() override;
    
    static bool supported();
    void reconfigure(ReconfigureFlags flags) override;
    
private Q_SLOTS:
    void onPenProximityIn(const QPointF &position);
    void onPenProximityOut();
    void onPenPositionChanged(const QPointF &position);
    void onPenTipDown();
    void onPenTipUp();
    void onCursorShapeChanged();
    
private:
    bool shouldShowOverlay() const;
    void updateOverlayVisibility();
    
    std::unique_ptr<PenInputListener> m_monitor;
    std::unique_ptr<OverlayRenderer> m_overlay;
    bool m_penInProximity = false;
    bool m_penTipDown = false;
    QString m_cursorPath;
};

}

#endif
