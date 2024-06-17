#include <Box2D/Box2D.h>
#include <cairo.h>

class CairoDebugDraw : public b2Draw {
public:
    void SetCR(cairo_t *_cr) {
        this->cr = _cr;
    }

    void DrawPolygon(const b2Vec2 *vertices, int32 vertexCount, const b2Color &color) override {
        cairo_set_source_rgba(cr, color.r, color.g, color.b, color.a);
        cairo_move_to(cr, vertices[0].x, vertices[0].y);
        for (int i = 1; i < vertexCount; ++i) {
            cairo_line_to(cr, vertices[i].x, vertices[i].y);
        }
        cairo_close_path(cr);
        cairo_stroke(cr);
    }

    void DrawSolidPolygon(const b2Vec2 *vertices, int32 vertexCount, const b2Color &color) override {
        cairo_set_source_rgba(cr, color.r, color.g, color.b, color.a * 0.5); // Semi-transparent fill
        cairo_move_to(cr, vertices[0].x, vertices[0].y);
        for (int i = 1; i < vertexCount; ++i) {
            cairo_line_to(cr, vertices[i].x, vertices[i].y);
        }
        cairo_close_path(cr);
        cairo_fill_preserve(cr);
        cairo_stroke(cr);
    }

    void DrawCircle(const b2Vec2 &center, float radius, const b2Color &color) override {
        cairo_set_source_rgba(cr, color.r, color.g, color.b, color.a);
        cairo_arc(cr, center.x, center.y, radius, 0, 2 * M_PI);
        cairo_stroke(cr);
    }

    void DrawSolidCircle(const b2Vec2 &center, float radius, const b2Vec2 &axis, const b2Color &color) override {
        cairo_set_source_rgba(cr, color.r, color.g, color.b, color.a * 0.5); // Semi-transparent fill
        cairo_arc(cr, center.x, center.y, radius, 0, 2 * M_PI);
        cairo_fill_preserve(cr);
        cairo_stroke(cr);

        b2Vec2 endPoint = center + radius * axis;
        cairo_move_to(cr, center.x, center.y);
        cairo_line_to(cr, endPoint.x, endPoint.y);
        cairo_stroke(cr);
    }

    void DrawSegment(const b2Vec2 &p1, const b2Vec2 &p2, const b2Color &color) override {
        cairo_set_source_rgba(cr, color.r, color.g, color.b, color.a);
        cairo_move_to(cr, p1.x, p1.y);
        cairo_line_to(cr, p2.x, p2.y);
        cairo_stroke(cr);
    }

    void DrawTransform(const b2Transform &xf) override {
        const float k_axisScale = 2.5f;

        cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 1.0); // Red for X axis
        cairo_move_to(cr, xf.p.x, xf.p.y);
        cairo_line_to(cr, xf.p.x + k_axisScale * xf.q.GetXAxis().x, xf.p.y + k_axisScale * xf.q.GetXAxis().y);
        cairo_stroke(cr);

        cairo_set_source_rgba(cr, 0.0, 1.0, 0.0, 1.0); // Green for Y axis
        cairo_move_to(cr, xf.p.x, xf.p.y);
        cairo_line_to(cr, xf.p.x + k_axisScale * xf.q.GetYAxis().x, xf.p.y + k_axisScale * xf.q.GetYAxis().y);
        cairo_stroke(cr);
    }

    void DrawPoint(const b2Vec2 &p, float size, const b2Color &color) override {
        cairo_set_source_rgba(cr, color.r, color.g, color.b, color.a);
        cairo_arc(cr, p.x, p.y, size, 0, 2 * M_PI);
        cairo_fill(cr);
    }

private:
    cairo_t *cr;
};
