#include "makise_e.h"

uint8_t _m_slider_draw   (MElement* b);
MInputResultEnum _m_slider_input  (MElement* b, MInputData data);
MFocusEnum _m_slider_focus  (MElement* b, MFocusEnum act);

void m_create_slider(MSlider* b, MContainer *c,
		     int32_t x, int32_t y, uint32_t w, uint32_t h,
		     char* text,
		     int32_t *value,
		     int32_t value_max,
		     int32_t value_min,
		     uint8_t show_val,
		     void    (*onchange   )(MSlider* b, uint32_t val),
		     void    (*onfocus )(MSlider* b, MFocusEnum type),
		     MakiseStyle *style)
{
    MElement *e = &b->el;
    e->gui = c->gui;

    e->data = b;

    e->draw = &_m_slider_draw;
    e->predraw = 0;
    e->update = 0;
    e->input = &_m_slider_input;
    e->focus = &_m_slider_focus;
    e->free = 0;
    
    e->position.x = x;
    e->position.y = y;
    e->position.width = w;
    e->position.height = h;

    e->enabled = 1;
    e->id = makise_g_newid();
    
    e->focus_prior = 1;
    
    b->text = text;
    b->value = value;
    b->value_max = value_max;
    b->value_min = value_min;
    if(*b->value > value_max)
	*b->value = value_max;
    if(*b->value < value_min)
	*b->value = value_min;
    
    b->show_value = show_val;

    
    b->onchange = onchange;
    b->onfocus = onfocus;
    
    b->style = style;
    
    makise_g_cont_add(c, e);
    
    printf("Slider %d created\n", e->id);
}

uint8_t _m_slider_draw   (MElement* b)
{
    MakiseStyleTheme *th = 0;

    MSlider *s = ((MSlider*)b->data);
    
    if(((MSlider*)b->data)->state == 0)
	th = &((MSlider*)b->data)->style->normal;
    else if(((MSlider*)b->data)->state == 1)
	th = &((MSlider*)b->data)->style->focused;

    _m_e_helper_draw_box(b->gui->buffer, &b->position, th);

    if(_MSlider_is_horizontal(b))
    {
	int32_t val = (*s->value - s->value_min) * (b->position.width - 5) /
	    (s->value_max - s->value_min);
	//borders
	makise_d_rect_filled(b->gui->buffer,
			     b->position.x,
			     b->position.y,
			     3,
			     b->position.height,
			     th->font_col,
			     ((MSlider*)b->data)->state ? th->font_col : th->bg_color);
	makise_d_rect_filled(b->gui->buffer,
			     b->position.x + b->position.width - 3,
			     b->position.y,
			     3,
			     b->position.height,
			     th->font_col,
			     ((MSlider*)b->data)->state ? th->font_col : th->bg_color);
	//filled part
	makise_d_rect_filled(b->gui->buffer,
			     b->position.x + 2,
			     b->position.y + b->position.height/6,
			     val,
			     b->position.height*4/6,
			     th->font_col,
			     ((MSlider*)b->data)->state ? th->font_col : th->bg_color);
	//empty part
	makise_d_rect(b->gui->buffer,
			     b->position.x + val + 3,
			     b->position.y + b->position.height/6,
			     b->position.width - val - 5,
			     b->position.height*4/6,
			     th->font_col);
	//line
	makise_d_line(b->gui->buffer,
		      b->position.x + val + 2,
		      b->position.y,
		      b->position.x + val + 2,
		      b->position.y + b->position.height - 1,
		      th->font_col);

    }
    
    //makise_d_string(b->gui->buffer,
//		    ((MSlider*)b->data)->text, MDTextAll,
//		    b->position.real_x + b->position.width / 2,
//		    b->position.real_y + b->position.height / 2,
//		    MDTextPlacement_Center,
//		    ((MSlider*)b->data)->style->font,
//		    th->font_col);

    //printf("Slider %d dr\n", b->id);
    return M_OK;
}

MInputResultEnum _m_slider_input  (MElement* b, MInputData data)
{
    printf("but click %d %d\n", b->id, b->position.real_y);
    MSlider *e = ((MSlider*)b->data);

    if(data.event == M_INPUT_CLICK || (e->value_max - e->value_min) > 20)
    {
	if(data.key == (_MSlider_is_horizontal(b) ? M_KEY_RIGHT : M_KEY_UP))
	{
	    if(*e->value + 1 > e->value_max)
		*e->value = e->value_max;
	    else
		(*e->value) ++;
	    return M_INPUT_HANDLED;
	}
	else if(data.key == (_MSlider_is_horizontal(b) ? M_KEY_LEFT : M_KEY_DOWN))
	{
	    if(*e->value - 1 < e->value_min)
		*e->value = e->value_min;
	    else
		(*e->value) --;
	    return M_INPUT_HANDLED;
	}
    }
    return M_INPUT_NOT_HANDLED;
}
MFocusEnum _m_slider_focus  (MElement* b, MFocusEnum act)
{
    MSlider *e = ((MSlider*)b->data);
    if(act & M_G_FOCUS_GET)
    {
	if(e->state != 1 && e->onfocus != 0)
	{
	    e->onfocus(e, M_G_FOCUS_GET);
	}
	((MSlider*)b->data)->state = 1;
    }
    if(act == M_G_FOCUS_LEAVE)
    {
	if(e->state != 0 && e->onfocus != 0)
	{
	    e->onfocus(e, M_G_FOCUS_LEAVE);
	}
	((MSlider*)b->data)->state = 0;
    }

    return (act == M_G_FOCUS_PREV || act == M_G_FOCUS_NEXT)
	? M_G_FOCUS_NOT_NEEDED
	: M_G_FOCUS_OK;
}

