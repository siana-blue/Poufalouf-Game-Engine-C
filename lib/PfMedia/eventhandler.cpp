#include "eventhandler.h"

EventHandler::EventHandler() : m_leftMouse(EVENT_RELEASED), m_rightMouse(EVENT_RELEASED), m_mouseRelX(0), m_mouseRelY(0), m_quitting(false),
    m_caret(false), m_umlaut(false), m_tilde(false) {}

void EventHandler::pollEvents()
{
    // Initialisation

	m_mouseRelX = m_mouseRelY = 0;
	vector<SDL_Keycode> codeToErase_v;
	for (map<SDL_Keycode, EventHandlerCode>::iterator it=m_keys_map.begin();it!=m_keys_map.end();++it)
	{
		if (it->second == EVENT_JUST_PRESSED)
			it->second = EVENT_PRESSED;
		else if (it->second == EVENT_JUST_RELEASED)
			codeToErase_v.push_back(it->first);
	}
	for (unsigned int i=0, size=codeToErase_v.size();i<size;i++)
		m_keys_map.erase(codeToErase_v[i]);
	if (m_leftMouse == EVENT_JUST_PRESSED)
		m_leftMouse = EVENT_PRESSED;
	else if (m_leftMouse == EVENT_JUST_RELEASED)
		m_leftMouse = EVENT_RELEASED;
	if (m_rightMouse == EVENT_JUST_PRESSED)
		m_rightMouse = EVENT_PRESSED;
	else if (m_rightMouse == EVENT_JUST_RELEASED)
		m_rightMouse = EVENT_RELEASED;
	m_inputText_v.clear();

	// Traitement des événements SDL

	unsigned char c;
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
	    c = 0;
		switch (event.type)
		{
			case SDL_KEYDOWN:
				m_keys_map[event.key.keysym.sym] = EVENT_JUST_PRESSED;
				c = handleTextKey(event.key.keysym.scancode);
				if (c > 0)
                    m_inputText_v.push_back(c);
				break;
			case SDL_KEYUP:
				m_keys_map[event.key.keysym.sym] = EVENT_JUST_RELEASED;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT)
					m_leftMouse = EVENT_JUST_PRESSED;
				else if (event.button.button == SDL_BUTTON_RIGHT)
					m_rightMouse = EVENT_JUST_PRESSED;
				break;
			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT)
					m_leftMouse = EVENT_JUST_RELEASED;
				else if (event.button.button == SDL_BUTTON_RIGHT)
					m_rightMouse = EVENT_JUST_RELEASED;
				break;
			case SDL_MOUSEMOTION:
				m_mouseCoord = PfPoint((float) event.motion.x, (float) event.motion.y);
				m_mouseRelX += event.motion.xrel;
				m_mouseRelY += event.motion.yrel;
				break;
			case SDL_QUIT:
				m_quitting = true;
				break;
			default:
				break;
		}
	}
}

void EventHandler::reset()
{
	m_keys_map.clear();
	m_leftMouse = m_rightMouse = EVENT_RELEASED;
	m_mouseCoord = PfPoint(0.0, 0.0);
	m_mouseRelX = m_mouseRelY = 0;
	m_quitting = false;
	m_inputText_v.clear();
}

bool EventHandler::isKeyPressed(int key, bool justNow) const
{
	map<SDL_Keycode, EventHandlerCode>::const_iterator it = m_keys_map.find((SDL_Keycode) key);
	if (it == m_keys_map.end())
		return false;

	if (justNow)
		return (it->second == EVENT_JUST_PRESSED);
	else
		return (it->second == EVENT_JUST_PRESSED || it->second == EVENT_PRESSED);

	return false; // pour le compilateur
}

bool EventHandler::isKeyReleased(int key, bool justNow) const
{
	map<SDL_Keycode, EventHandlerCode>::const_iterator it = m_keys_map.find((SDL_Keycode) key);
	if (it == m_keys_map.end())
		return (!justNow); // normalement EVENT_RELEASED = retiré de la map

	if (justNow)
		return (it->second == EVENT_JUST_RELEASED);
	else
		return (it->second == EVENT_JUST_RELEASED || it->second == EVENT_RELEASED);

	return false; // pour le compilateur
}

bool EventHandler::isLeftMouseButtonClicked(bool justNow) const
{
	if (justNow)
		return (m_leftMouse == EVENT_JUST_PRESSED);
	else
		return (m_leftMouse == EVENT_JUST_PRESSED || m_leftMouse == EVENT_PRESSED);

	return false; // pour le compilateur
}

bool EventHandler::isLeftMouseButtonReleased(bool justNow) const
{
	if (justNow)
		return (m_leftMouse == EVENT_JUST_RELEASED);
	else
		return (m_leftMouse == EVENT_JUST_RELEASED || m_leftMouse == EVENT_RELEASED);

	return false; // pour le compilateur
}

bool EventHandler::isRightMouseButtonClicked(bool justNow) const
{
	if (justNow)
		return (m_rightMouse == EVENT_JUST_PRESSED);
	else
		return (m_rightMouse == EVENT_JUST_PRESSED || m_rightMouse == EVENT_PRESSED);

	return false; // pour le compilateur
}

bool EventHandler::isRightMouseButtonReleased(bool justNow) const
{
	if (justNow)
		return (m_rightMouse == EVENT_JUST_RELEASED);
	else
		return (m_rightMouse == EVENT_JUST_RELEASED || m_rightMouse == EVENT_RELEASED);

	return false; // pour le compilateur
}

unsigned char EventHandler::handleTextKey(SDL_Scancode code)
{
    SDL_Keycode kCode = SDL_GetKeyFromScancode(code);
    bool shift = (isKeyPressed(SDLK_LSHIFT) || isKeyPressed(SDLK_RSHIFT));
    bool alt = (isKeyPressed(SDLK_LALT) || isKeyPressed(SDLK_RALT));
    unsigned char x = 0;

    if (kCode == SDLK_SPACE)
        x = 32;
    else if (kCode == SDLK_a)
    {
        if (shift)
            x = 65;
        else if (m_caret)
            x = 131;
        else if (m_umlaut)
            x = 132;
        else
            x = 97;
    }
    else if (kCode == SDLK_e)
    {
        if (shift)
            x = 69;
        else if (m_caret)
            x = 136;
        else if (m_umlaut)
            x = 137;
        else
            x = 101;
    }
    else if (kCode == SDLK_i)
    {
        if (shift)
            x = 73;
        else if (m_caret)
            x = 140;
        else if (m_umlaut)
            x = 139;
        else
            x = 105;
    }
    else if (kCode == SDLK_o)
    {
        if (shift)
            x = 79;
        else if (m_caret)
            x = 147;
        else if (m_umlaut)
            x = 148;
        else
            x = 111;
    }
    else if (kCode == SDLK_u)
    {
        if (shift)
            x = 85;
        else if (m_caret)
            x = 150;
        else if (m_umlaut)
            x = 129;
        else
            x = 117;
    }
    else if (kCode == SDLK_n)
    {
        if (shift)
            x = 78;
        else if (m_tilde)
            x = 164;
        else
            x = 110;
    }
    else if (kCode >= SDLK_a && kCode <= SDLK_z)
    {
        if (shift)
            x = 65 + (kCode - SDLK_a);
        else
            x = 97 + (kCode - SDLK_a);
    }
    else if (kCode == SDLK_KP_0)
    {
        x = 48;
    }
    else if (kCode >= SDLK_KP_1 && kCode <= SDLK_KP_9)
    {
        x = 49 + (kCode - SDLK_KP_1);
    }
    else if (kCode >= SDLK_0 && kCode <= SDLK_9)
    {
        if (shift)
            x = 48 + (kCode - SDLK_0);
        else
        {
            switch (kCode)
            {
                case SDLK_0:
                    x = alt?64:133;
                    break;
                case SDLK_1:
                    x = 38;
                    break;
                case SDLK_2:
                    if (alt)
                    {
                        if (m_tilde)
                            x = 126;
                        m_umlaut = false;
                        m_caret = false;
                        m_tilde = !m_tilde;
                        return x;
                    }
                    x = 130;
                    break;
                case SDLK_3:
                    x = alt?35:34;
                    break;
                case SDLK_4:
                    x = alt?123:39;
                    break;
                case SDLK_5:
                    x = alt?91:40;
                    break;
                case SDLK_6:
                    x = alt?124:45;
                    break;
                case SDLK_7:
                    x = alt?96:138;
                    break;
                case SDLK_8:
                    x = alt?92:95;
                    break;
                case SDLK_9:
                    if (alt)
                    {
                        if (m_caret)
                            x = 94;
                        m_umlaut = false;
                        m_caret = !m_caret;
                        m_tilde = false;
                        return x;
                    }
                    x = 135;
                    break;
                default:
                    break;
            }
        }
    }
    else
    {
        if (shift)
        {
            switch (kCode)
            {
                case SDLK_COMMA:
                    x = 63;
                    break;
                case SDLK_SEMICOLON:
                    x = 46;
                    break;
                case SDLK_COLON:
                    x = 47;
                    break;
                case SDLK_LESS:
                    x = 62;
                    break;
                default:
                    break;
            }

            switch (code)
            {
                case SDL_SCANCODE_MINUS:
                    x = 167;
                    break;
                case SDL_SCANCODE_EQUALS:
                    x = 43;
                    break;
                case SDL_SCANCODE_LEFTBRACKET:
                    m_caret = false;
                    m_tilde = false;
                    m_umlaut = !m_umlaut;
                    return 0;
                case SDL_SCANCODE_APOSTROPHE:
                    x = 37;
                    break;
                default:
                    break;
            }
        }
        else if (alt)
        {
            switch (code)
            {
                case SDL_SCANCODE_MINUS:
                    x = 93;
                    break;
                case SDL_SCANCODE_EQUALS:
                    x = 125;
                    break;
                default:
                    break;
            }
        }
        else
        {
            switch (kCode)
            {
                case SDLK_DOLLAR:
                    x = 36;
                    break;
                case SDLK_ASTERISK:
                    x = 42;
                    break;
                case SDLK_COMMA:
                    x = 44;
                    break;
                case SDLK_SEMICOLON:
                    x = 59;
                    break;
                case SDLK_COLON:
                    x = 58;
                    break;
                case SDLK_EXCLAIM:
                    x = 33;
                    break;
                case SDLK_LESS:
                    x = 60;
                    break;
                default:
                    break;
            }

            switch (code)
            {
                case SDL_SCANCODE_GRAVE:
                    x = 166;
                    break;
                case SDL_SCANCODE_MINUS:
                    x = 41;
                    break;
                case SDL_SCANCODE_EQUALS:
                    x = 61;
                    break;
                case SDL_SCANCODE_LEFTBRACKET:
                    if (m_caret)
                        x = 94;
                    m_umlaut = false;
                    m_tilde = false;
                    m_caret = !m_caret;
                    return x;
                case SDL_SCANCODE_APOSTROPHE:
                    x = 151;
                    break;
                default:
                    break;
            }
        }
    }

    m_umlaut = m_caret = m_tilde = false;
    return x;
}
