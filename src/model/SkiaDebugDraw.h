#pragma once

#include <box2d/box2d.h>
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#include "include/core/SkPoint.h"

class SkiaDebugDraw : public b2Draw {
public:
    SkiaDebugDraw(SkCanvas *canvas) : m_canvas(canvas) {
        m_strokePaint.setStyle(SkPaint::kStroke_Style);
        m_strokePaint.setAntiAlias(true);

        m_fillPaint.setStyle(SkPaint::kFill_Style);
        m_fillPaint.setAntiAlias(true);
    }

    void DrawPolygon(const b2Vec2 *vertices, int32 vertexCount, const b2Color &color) override {
        SkPath path;
        path.moveTo(vertices[0].x, vertices[0].y);
        for (int i = 1; i < vertexCount; ++i) {
            path.lineTo(vertices[i].x, vertices[i].y);
        }
        path.close();

        m_strokePaint.setColor(SkColorSetARGB(255, color.r * 255, color.g * 255, color.b * 255));
        m_canvas->drawPath(path, m_strokePaint);
    }

    void DrawSolidPolygon(const b2Vec2 *vertices, int32 vertexCount, const b2Color &color) override {
        SkPath path;
        path.moveTo(vertices[0].x, vertices[0].y);
        for (int i = 1; i < vertexCount; ++i) {
            path.lineTo(vertices[i].x, vertices[i].y);
        }
        path.close();

        m_fillPaint.setColor(SkColorSetARGB(255, color.r * 255, color.g * 255, color.b * 255));
        m_canvas->drawPath(path, m_fillPaint);

        m_strokePaint.setColor(SkColorSetARGB(255, color.r * 255, color.g * 255, color.b * 255));
        m_canvas->drawPath(path, m_strokePaint);
    }

    void DrawCircle(const b2Vec2 &center, float radius, const b2Color &color) override {
        m_strokePaint.setColor(SkColorSetARGB(255, color.r * 255, color.g * 255, color.b * 255));
        m_canvas->drawCircle(center.x, center.y, radius, m_strokePaint);
    }

    void DrawSolidCircle(const b2Vec2 &center, float radius, const b2Vec2 &axis, const b2Color &color) override {
        m_fillPaint.setColor(SkColorSetARGB(255, color.r * 255, color.g * 255, color.b * 255));
        m_canvas->drawCircle(center.x, center.y, radius, m_fillPaint);

        m_strokePaint.setColor(SkColorSetARGB(255, color.r * 255, color.g * 255, color.b * 255));
        m_canvas->drawCircle(center.x, center.y, radius, m_strokePaint);

        b2Vec2 endPoint = center + radius * axis;
        m_canvas->drawLine(center.x, center.y, endPoint.x, endPoint.y, m_strokePaint);
    }

    void DrawSegment(const b2Vec2 &p1, const b2Vec2 &p2, const b2Color &color) override {
        m_strokePaint.setColor(SkColorSetARGB(255, color.r * 255, color.g * 255, color.b * 255));
        m_canvas->drawLine(p1.x, p1.y, p2.x, p2.y, m_strokePaint);
    }

    void DrawTransform(const b2Transform &xf) override {
        const float k_axisScale = 0.4f;
        b2Vec2 p1 = xf.p, p2;

        // Red (x-axis)
        p2 = p1 + k_axisScale * xf.q.GetXAxis();
        m_strokePaint.setARGB(255, 255, 0, 0);
        m_canvas->drawLine(p1.x, p1.y, p2.x, p2.y, m_strokePaint);

        // Green (y-axis)
        p2 = p1 + k_axisScale * xf.q.GetYAxis();
        m_strokePaint.setARGB(255, 0, 255, 0);
        m_canvas->drawLine(p1.x, p1.y, p2.x, p2.y, m_strokePaint);
    }

    void DrawPoint(const b2Vec2 &p, float size, const b2Color &color) override {
        m_strokePaint.setColor(SkColorSetARGB(255, color.r * 255, color.g * 255, color.b * 255));
        m_canvas->drawCircle(p.x, p.y, size, m_strokePaint);
    }

private:
    SkCanvas *m_canvas;
    SkPaint m_strokePaint;
    SkPaint m_fillPaint;
};
