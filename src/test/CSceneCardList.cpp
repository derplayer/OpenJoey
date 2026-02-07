// CSceneCardList.cpp
// Created by derplayer
// Created on 2025-05-23 10:30:43

#include "CSceneCardList.h"
#include <fstream>
#include <sstream>

// Destructor for manual cleanup of CGUIButton pointers
CSceneCardList::~CSceneCardList() {
    // These were allocated with 'new', so they must be 'delete'd.
    delete m_backButton;
    m_backButton = NULL; // Prevent double delete
    delete m_prevPageButton;
    m_prevPageButton = NULL;
    delete m_nextPageButton;
    m_nextPageButton = NULL;
}

void CSceneCardList::OnInit() {
    // Initialize input
    m_mouse.Flush();
    m_mouse.SetGuardTime(1);

	// Load card bin system
	m_bin = app->GetBinSystem();

    // Init counters and timers
    m_nFade = yaneuraoGameSDK3rd::Math::CSaturationCounter(0, 255, 8);
    nFadeBG.Set(0, 255, 16);
    m_timerMain = CTimer();
    m_timerMain.Restart();
    m_timerMain.Reset();

    // Clone fb from previous scene if needed
    smart_ptr<ISurface> screenPtr = app->GetDraw()->GetSecondary()->cloneFull();
    if (screenPtr.get()) {
        m_background = screenPtr;
        // OutputDebugStringA("Framebuffer data cloned and loaded successfully!\n"); // Commented out for performance
    }

    // Initialize scene animation state
    m_sceneAnimState = SAS_INITIAL_LOAD; // Start with the grid entering animation
    m_bPageChangeRequested = false;
    m_bForwardPageChange = false;

    // NEW: Initialize scene frame counter
    m_currentSceneFrameCount = 0; // Start at 0

    // Initialize page tracking
    m_nCurrentPage = 1;
	m_nPreviewCard = NULL;
    m_nPreviewCardId = 0; // Initialize to 0, means no card is previewed

    // Load scene layout data
    m_vPlaneLoader.SetLang(app->GetLang());
    m_vPlaneLoader.SetReadDir("data/y/list/");
    if (m_vPlaneLoader.Set("data/y/list/list_scene.txt", false) != 0) {
        OutputDebugStringA("Error: Failed to load data/y/list/list_scene.txt\n");
    }
    // FIX: Set color key for transparency on m_vPlaneLoader
    m_vPlaneLoader.SetColorKey(ISurface::makeRGB(0, 255, 0, 128)); // alpha value because transparency logic will mismatch, code is expecting RGBA and not RGB


    // Load detail layout data
    m_vDetailPlaneLoader.SetLang(app->GetLang());
    m_vDetailPlaneLoader.SetReadDir("data/y/list/");
    if (m_vDetailPlaneLoader.Set("data/y/list/list_detail.txt", false) != 0) {
        OutputDebugStringA("Error: Failed to load data/y/list/list_detail.txt\n");
    }

	// Load m_fullCardPreviewPlane from card_ura.bmp
	smart_ptr<CFastPlane> uraDefaultPlane(new CFastPlane());
	if (uraDefaultPlane->Load("data/card/card_ura.bmp") == 0) {
		m_fullCardPreviewPlaneUra = uraDefaultPlane;
	}

    InitializeUI();
    LoadCardData(); // This will now also create the CGUUIButton for each card
    LoadCardTexturesForCurrentPage(); // Load initial page textures (and associate with buttons)

    // Initialize individual card animations for the first page
    ResetCardAnimations(true); // Prepare for initial grid entry animation
}

// Corrected SetHoverButtonPlane call sites
void CSceneCardList::SetHoverButtonPlane(CGUIButton* btn, int id, bool negativeOrder) {
    CGUIButtonEventListener* e = btn->GetEvent().get();
    // Use static_cast for safer downcasting from base to derived class
    CGUINormalButtonListener* p = static_cast<CGUINormalButtonListener*>(e);
    if (btn->IsIn())
        if (negativeOrder)
            p->SetPlaneNumber(id - 1);
        else
            p->SetPlaneNumber(id + 1);
    else
        p->SetPlaneNumber(id);
}

void CSceneCardList::OnMove(const smart_ptr<ISurface>& lp) {
    key.Input();
    m_mouse.Flush();

    // Increment scene frame counter
    m_currentSceneFrameCount++; // This is crucial for the new animation timing
    if (m_timerMain.Get() < 500) return;

    // Update buttons
    if (m_backButton) {
        m_backButton->OnSimpleMove(lp.get());
        SetHoverButtonPlane(m_backButton, 1, false); // Explicitly pass false
        if (m_backButton->IsLClick()) {
            GetSceneControl()->ReturnScene();
        }
    }

    // Handle page navigation
    if (m_prevPageButton) {
        m_prevPageButton->OnSimpleMove(lp.get());
        SetHoverButtonPlane(m_prevPageButton, 6, true);
        if (m_prevPageButton->IsLClick() && m_nCurrentPage > 1 && m_sceneAnimState == SAS_IDLE) {
            m_bPageChangeRequested = true;
            m_bForwardPageChange = false;
            m_sceneAnimState = SAS_EXITING_GRID; // Start exit animation
			m_nPreviewCard = NULL;
            m_nPreviewCardId = 0; // Clear preview on page change
            m_fullCardPreviewPlane = smart_ptr<CFastPlane>(); // Clear full card preview
        }
    }

    if (m_nextPageButton) {
        m_nextPageButton->OnSimpleMove(lp.get());
        SetHoverButtonPlane(m_nextPageButton, 8, true);
        if (m_nextPageButton->IsLClick() && m_nCurrentPage < m_nTotalPages && m_sceneAnimState == SAS_IDLE) {
            m_bPageChangeRequested = true;
            m_bForwardPageChange = true;
            m_sceneAnimState = SAS_EXITING_GRID; // Start exit animation
			m_nPreviewCard = NULL;
            m_nPreviewCardId = 0; // Clear preview on page change
            m_fullCardPreviewPlane = smart_ptr<CFastPlane>(); // FIX: Clear full card preview
        }
    }

    // Update individual card buttons for current page
    int startIndex = (m_nCurrentPage - 1) * CARDS_PER_PAGE;
    int endIndex = startIndex + CARDS_PER_PAGE;
    if ((size_t)endIndex > m_ownedCards.size()) {
        endIndex = m_ownedCards.size();
    }

    // Only allow card interaction when grid is idle
    if (m_sceneAnimState == SAS_IDLE) {
        for (int col = 0; col < CARD_COLUMNS; col++) {
            for (int row = 0; row < CARD_ROWS; row++) {
                int collectionIndex = startIndex + (col * CARD_ROWS) + row;

                if (collectionIndex >= 0 && (size_t)collectionIndex < m_ownedCards.size()) {
                    CardInfo& currentCard = m_ownedCards[collectionIndex];
                    if (currentCard.m_cardButton.get()) {
                        currentCard.m_cardButton->OnSimpleMove(lp.get()); // Update card button state

						if (currentCard.m_cardButton->IsLClick() || currentCard.m_cardButton->IsIn()) {
							
							m_nPreviewCardId = currentCard.id; // Set the clicked card's ID for preview
							m_nPreviewCard = currentCard.cardData;

                            // Load the full card graphic
                            char fullCardPath[256];
                            sprintf(fullCardPath, "data/card/%s.bmp", currentCard.bmpName.c_str());
                            smart_ptr<CFastPlane> fullPlane(new CFastPlane());
                            if (fullPlane->Load(fullCardPath) == 0) {
                                m_fullCardPreviewPlane = fullPlane;
                                OutputDebugStringA("Loaded full card preview: ");
                                OutputDebugStringA(fullCardPath);
                                OutputDebugStringA("\n");
                            } else {
                                m_fullCardPreviewPlane = smart_ptr<CFastPlane>(); // FIX: Clear if load failed
                                OutputDebugStringA("Failed to load full card preview: ");
                                OutputDebugStringA(fullCardPath);
                                OutputDebugStringA("\n");
                            }
                        }
                    }
                }
            }
        }
    }


    // Update card animations based on scene state
    UpdateCardAnimations();

	// Update the textbox draw state (e.g., mouse interaction, scrolling)
	if (m_cardTextBox.get()) {
		m_cardTextBox->OnSimpleMove(lp.get());
	}

	// Update the textbox draw state (e.g., mouse interaction, scrolling)
	if (m_cardTextBoxFooter.get()) {
		m_cardTextBox->OnSimpleMove(lp.get());
	}
}

void CSceneCardList::OnDraw(const smart_ptr<ISurface>& lp) {
    // Draw background
    if (m_bgPlane) {
        ISurfaceTransBlt::CircleBlt5(lp.get(), m_bgPlane.get(), 0, 0, (int)nFadeBG, 0, 255);
        nFadeBG.Inc();
    }

    // Draw title with fade effect
    if (m_timerMain.Get() > 0 && m_titlePlane) {
        lp->BlendBltFast(m_titlePlane.get(), m_titlePlane->GetPosX(), m_titlePlane->GetPosY(), m_nFade);
    }

    if (m_timerMain.Get() > 500) {
        // Draw page navigation buttons
        if (m_prevPageButton) {
            m_prevPageButton->OnSimpleDraw(lp.get());
        }
        if (m_nextPageButton) {
            m_nextPageButton->OnSimpleDraw(lp.get());
        }

        // Draw back button
        if (m_backButton) {
            m_backButton->OnSimpleDraw(lp.get());
        }

        // Draw card grid (logic will now consider animation state)
        DrawCardGrid(lp);

        // Draw card preview (now uses m_fullCardPreviewPlane)
        DrawCardPreview(lp);

        // Draw pagination
        DrawPagination(lp);

        // Draw collection rate
        DrawCollectionRate(lp);

		// Draw the card textbox
		if (m_cardTextBox.get()) {
			m_cardTextBox->OnSimpleDraw(lp.get());
		}

		// Draw the card textbox footer
		if (m_cardTextBoxFooter.get()) {

			if(m_nPreviewCardId != 0)
			{
				MonsterType ctype = m_nPreviewCard->properties.GetMonsterType();
				if (ctype == TYPE_SPELLCARD || ctype == TYPE_TRAPCARD)
				{
					// INFO: dont draw footer for spell and trap cards
				}
				else 
				{
					// Draw pap line and overlay
					// TODO: cache this data in memory, geez
					POINT papPos = m_vDetailPlaneLoader.GetXY(3); // Only X axis is defined
					int textBoxX, textBoxY;
					m_cardTextBox->GetSize(textBoxX, textBoxY);
					int textBoxPosX, textBoxPosY;
					m_cardTextBox->GetXY(textBoxPosX, textBoxPosY);
					int arrowX, arrowY;
					m_cardTextBox->GetArrowSize(arrowX, arrowY);
					int papYAxis = (textBoxY+textBoxPosY) - arrowY;

					lp->BltFast(m_papLine.get(), papPos.x, papYAxis); // blit pap separator

					// Get pap length
					int papLX, papLY;
					m_papLine->GetSize(papLX, papLY);

					// Prepare background reblit to hide text behind it
					int startX = papPos.x;
					int startY = papYAxis + 1; // 1 is the line thinckness
					int sliceW = papLX; // Only pap x-axis size needed
					int sliceH = papPos.x;

					// RECT is { left, top, right, bottom }
					RECT sourceRect = {
						startX, 
						startY, 
						startX + sliceW, 
						startY + sliceH 
					};

					// Blit the snippet to the exact same screen position to "restore" the background
					lp->BltFast(
						m_bgPlane, 
						startX, 
						startY, 
						NULL, 
						&sourceRect, 
						NULL, 
						0
					);

					m_cardTextBoxFooter->OnSimpleDraw(lp.get());
				}
			}


		}
	}

    m_nFade.Inc();
}

void CSceneCardList::InitializeUI() {
    // Load background asset
    CPlane bgPlane = m_vDetailPlaneLoader.GetPlane(0); // list_bg.bmp
    POINT bgPos = { 0, 0 }; //HACK: this seems to be hardcoded in exe and the txt spec is not used? (POINT bgPos = m_vDetailPlaneLoader.GetXY(0);)
    bgPlane->SetPos(bgPos);
    m_bgPlane = bgPlane;

	// Load PAP line asset
    CPlane papPlane = m_vDetailPlaneLoader.GetPlane(3); // pap_line.bmp
	m_papLine = papPlane;

    // Create title (list_name_?.bmp based on language)
    CPlane titlePlane = m_vPlaneLoader.GetPlane(0);
    POINT titlePos = m_vPlaneLoader.GetXY(0);
    titlePlane->SetPos(titlePos);
    m_titlePlane = titlePlane;

    // Create back button
    CGUIButton* backBtn = new CGUIButton();
    backBtn->SetID(1);
    backBtn->SetMouse(smart_ptr<CFixMouse>(&m_mouse, false));
    smart_ptr<CGUIButtonEventListener> buttonListener(new CGUINormalButtonListener());
    CGUINormalButtonListener* p = static_cast<CGUINormalButtonListener*>(buttonListener.get()); // Use static_cast
    p->SetPlaneLoader(smart_ptr<CPlaneLoader>(&m_vPlaneLoader, false), 1);

    POINT backPos = m_vPlaneLoader.GetXY(1);
    backBtn->SetEvent(buttonListener);
    backBtn->SetXY(backPos.x, backPos.y);
    m_backButton = backBtn;

    // Get card hover border from plane loader
    m_cardHoverBorder = m_vPlaneLoader.GetPlane(9); // card_waku_s.bmp from list_scene.txt

    // Create preview card surface (this might still be used for background, but not for the actual image)
    m_cardPreviewImage.CreateSurface(200, 290, false);
    m_cardPreviewImage.SetFillColor(ISurface::makeRGB(0, 0, 0, 0));

    // Create page navigation arrows
    InitializePageControls();

	// Create card textbox
	m_cardTextBoxPLoader.SetReadDir("data/y/list/");  // Base directory
    if (m_cardTextBoxPLoader.Set("data/y/list/detail_scroll.txt", false) != 0) {  // Relative to SetReadDir
        OutputDebugStringA("Error: Failed to load data/y/list/detail_scroll.txt\n");
    }

	CPlane sliderBox = m_cardTextBoxPLoader.GetPlane(1);
	smart_ptr<ISurface> sliderSmartPtr(sliderBox.get(), false); // no ownership

	CPlane separatorLine = m_vDetailPlaneLoader.GetPlane(3);
	smart_ptr<ISurface> separatorLineSmartPtr(separatorLine.get(), false); // no ownership

	m_cardTextBox = smart_ptr<yaneuraoGameSDK3rd::Draw::CGUITextBox>(new yaneuraoGameSDK3rd::Draw::CGUITextBox(), false);
	m_cardTextBoxFooter = smart_ptr<yaneuraoGameSDK3rd::Draw::CGUITextBox>(new yaneuraoGameSDK3rd::Draw::CGUITextBox(), false);

	POINT topLeftBox = m_vDetailPlaneLoader.GetXY(1);
	POINT botRightBox = m_vDetailPlaneLoader.GetXY(2);
	int sliderX, sliderY;
	sliderBox->GetSize(sliderX, sliderY);

	// INFO: textbox widget works different so we dont really need those (but left here as comment for authenticity reasons)
	//std::string rawScrollData = m_cardTextBoxPLoader.GetXYRaw(0);
	//int thicknessData = CStringScanner::ConvertToInt(rawScrollData.substr(0,2)); // Thickness (2 digits)
	//int buttonData = CStringScanner::ConvertToInt(rawScrollData.substr(2,2)); // Button (2 digits)
	//int boxData = CStringScanner::ConvertToInt(rawScrollData.substr(4,2)); // Box (2 digits)
	//int barData = CStringScanner::ConvertToInt(rawScrollData.substr(6,3)); // Bar length (3 digits)
	
	int boxScaleX = (botRightBox.x - topLeftBox.x) + sliderX;
	int boxScaleY = (botRightBox.y - topLeftBox.y);

	// TODO: create signature with two XY points and calulate anything else from this
	m_cardTextBox->Create(topLeftBox.x, topLeftBox.y, boxScaleX, boxScaleY, yaneuraoGameSDK3rd::Draw::CGUITextBox::VERTICAL_SLIDER);

	m_cardTextBox->SetMouse(smart_ptr<CFixMouse>(&m_mouse, false)); // Pass the current mouse state
	m_cardTextBox->SetTextColor(yaneuraoGameSDK3rd::Draw::ISurface::makeRGB(0, 0, 0, 0));
	m_cardTextBox->SetSliderGFX(sliderSmartPtr);
	m_cardTextBox->SetArrowGFX(smart_ptr<CPlaneLoader>(&m_cardTextBoxPLoader, false), 5, 8);
	m_cardTextBox->SetMargins(4,4);

	// Footer init.
	int arrowSizeX, arrowSizeY;
	m_cardTextBox->GetArrowSize(arrowSizeX, arrowSizeY);
	int insideYAxisFactor = (topLeftBox.y + boxScaleY) - arrowSizeY; // scroll box negative padding is the final footer size
	m_cardTextBoxFooter->Create(topLeftBox.x, insideYAxisFactor, boxScaleX, boxScaleY, yaneuraoGameSDK3rd::Draw::CGUITextBox::VERTICAL_SLIDER);
	m_cardTextBoxFooter->SetMargins(0,2);

	// Default fonts init.
	// TODO: default font passtrough is broken at the moment (HTML tags are used for everything)

	//smart_ptr<yaneuraoGameSDK3rd::Draw::CFont> customFont(new yaneuraoGameSDK3rd::Draw::CFont());
	//customFont->SetFont("Arial");
	//customFont->SetSize(13);
	//customFont->SetWeight(FW_NORMAL);
	//customFont->SetItalic(false);
	//customFont->SetShadowOffset(0, 0);
	//customFont->SetLetterSpacing(-1);
	//customFont->SetColor(ISurface::makeRGB(0, 0, 0, 0)); // Black text (assuming last 0 is alpha for opaque)
	//customFont->SetColor(ISurface::makeRGB(26, 47, 75, 0)); // Dark brown text (BGR)
	//customFont->SetHeight(18); // Adjust for desired line spacing, e.g., 15-17 for 12pt font
	//m_cardTextBox->SetFont(customFont);

	//smart_ptr<yaneuraoGameSDK3rd::Draw::CFont> customFontF(new yaneuraoGameSDK3rd::Draw::CFont());
	//customFontF->SetFont("Arial");
	//customFontF->SetSize(13);
	//customFontF->SetWeight(FW_BOLD);
	//customFontF->SetItalic(false);
	//customFontF->SetShadowOffset(0, 0);
	//customFontF->SetLetterSpacing(-1);
	//customFontF->SetColor(ISurface::makeRGB(0, 0, 0, 0)); // Black text (assuming last 0 is alpha for opaque)
	//customFontF->SetColor(ISurface::makeRGB(26, 47, 75, 0)); // Dark brown text (BGR)
	//customFontF->SetHeight(18); // Adjust for desired line spacing, e.g., 15-17 for 12pt font
	//m_cardTextBoxFooter->SetFont(customFontF);
}

void CSceneCardList::InitializePageControls() {
    // Left arrow button
    CGUIButton* leftBtn = new CGUIButton();
    leftBtn->SetMouse(smart_ptr<CFixMouse>(&m_mouse, false));
    smart_ptr<CGUIButtonEventListener> leftListener(new CGUINormalButtonListener());
    CGUINormalButtonListener* pl = static_cast<CGUINormalButtonListener*>(leftListener.get()); // Use static_cast
    pl->SetPlaneLoader(smart_ptr<CPlaneLoader>(&m_vPlaneLoader, false), 6); // pe_yaji_l1.bmp, pe_yaji_l2.bmp etc.

    POINT leftPos = m_vPlaneLoader.GetXY(6);
    leftBtn->SetEvent(leftListener);
    leftBtn->SetXY(leftPos.x, leftPos.y);
    m_prevPageButton = leftBtn;

    // Right arrow button
    CGUIButton* rightBtn = new CGUIButton();
    rightBtn->SetMouse(smart_ptr<CFixMouse>(&m_mouse, false));
    smart_ptr<CGUIButtonEventListener> rightListener(new CGUINormalButtonListener());
    CGUINormalButtonListener* pr = static_cast<CGUINormalButtonListener*>(rightListener.get()); // Use static_cast
    pr->SetPlaneLoader(smart_ptr<CPlaneLoader>(&m_vPlaneLoader, false), 8); // pe_yaji_r1.bmp, pe_yaji_r2.bmp etc.

    POINT rightPos = m_vPlaneLoader.GetXY(8);
    rightBtn->SetEvent(rightListener);
    rightBtn->SetXY(rightPos.x, rightPos.y);
    m_nextPageButton = rightBtn;
}

void CSceneCardList::LoadCardData() {
    std::ifstream cardList("data/card/list_card.txt");
    if (!cardList.is_open()) {
        OutputDebugStringA("Error: Failed to open data/card/list_card.txt\n");
        return;
    }

    std::string line;
    while (std::getline(cardList, line)) {
        if (line.empty()) continue;
        if (!line.empty() && line[line.length() - 1] == '\r') {
            line.erase(line.length() - 1);
        }
        if (line.empty()) continue;

        int currentId = -1; // Reset for each potential card entry
        std::string currentBmp;

        // Check if it's an ID line (starts with // and contains [ID])
        if (line.substr(0, 2) == "//") {
			// TODO: we should parse the internal id from txt, not the real id (better perfomance, no lookup loop)
            size_t idStart = line.find("[");
            size_t idEnd = line.find("]");
            if (idStart != std::string::npos && idEnd != std::string::npos && idEnd > idStart) {
                std::string idStr = line.substr(idStart + 1, idEnd - idStart - 1);
                currentId = std::atoi(idStr.c_str());
                // The next non-comment line should be the BMP
                std::string nextLine;
                if (std::getline(cardList, nextLine)) {
                    if (!nextLine.empty() && nextLine[nextLine.length() - 1] == '\r') {
                        nextLine.erase(nextLine.length() - 1);
                    }
                    if (nextLine.find(".bmp") != std::string::npos) {
                        currentBmp = nextLine;
                        if (currentId != -1 && currentBmp != "card_ura.bmp") {
                            CardInfo card;
                            card.id = currentId;
                            card.templateId = currentId;
                            card.isNew = false;
                            card.bmpName = currentBmp.substr(0, currentBmp.length() - 4); // Remove .bmp extension

							// Match database entry with list entry
							WORD intId = m_bin->GetCardInternalId(currentId); // We parse only the internal id from the txt list so...
							card.cardData = m_bin->GetCard(intId); // load card metadata from db
							
                            // Initialize new animation members for each card with ORIGINAL speeds
                            card.m_nScaleAnimation.Set(0, 100, ORIGINAL_CARD_ANIM_SCALE_SPEED);
                            // Subtler Y-offset start value for the "drop-in" effect
                            card.m_nYOffsetAnimation.Set(-(CARD_HEIGHT / 8), 0, ORIGINAL_CARD_ANIM_SPEED);

                            card.m_bAnimationStarted = false;
                            card.m_bAnimationCompleted = false;

                            // NEW: Create CGUIButton for each card
                            CGUIButton* cardBtn = new CGUIButton(); // Allocated with new, owned by smart_ptr
                            cardBtn->SetID(card.id); // Use card ID as button ID
                            cardBtn->SetMouse(smart_ptr<CFixMouse>(&m_mouse, false));

                            smart_ptr<CGUIButtonEventListener> cardBtnListener(new CGUINormalButtonListener());
                            CGUINormalButtonListener* pBtn = static_cast<CGUINormalButtonListener*>(cardBtnListener.get());

                            // Create a CFastPlane for this card's image
                            smart_ptr<CFastPlane> cardPlane = smart_ptr<CFastPlane>(new CFastPlane());
                            char filepath[256];
                            sprintf(filepath, "data/mini/%s.bmp", card.bmpName.c_str());
                            if (cardPlane->Load(filepath) == 0) {
                                // Set the button's plane to the card's image
                                pBtn->SetPlane(smart_ptr<ISurface>(cardPlane.get(), false)); // Button listener doesn't own surface
                                cardBtn->SetEvent(cardBtnListener);
                                // Set position later in DrawCardGrid as it changes due to animation
                                card.m_cardButton = smart_ptr<CGUIButton>(cardBtn); // Assign smart_ptr ownership
                                m_ownedCards.push_back(card);
                                // Store the texture in map for quick lookup later if needed, though button now owns it visually
                                m_cardTextures[card.id] = cardPlane; // Keep in map for reference, or if direct blitting is still desired
                            } else {
                                delete cardBtn; // Clean up button if image failed to load
                                char debugStr[256];
                                sprintf(debugStr, "Warning: Failed to load card texture for button: %s\n", filepath);
                                OutputDebugStringA(debugStr);
                            }
                        }
                    }
                }
            }
        }
    }
    cardList.close();

    m_nTotalPages = (m_ownedCards.size() + CARDS_PER_PAGE - 1) / CARDS_PER_PAGE;
}

void CSceneCardList::ClearCardTextures() {
    // We are now relying on the CGUIButton's internal plane, so this might not be strictly necessary
    // for rendering, but good to clear if the textures are cached elsewhere.
    // If the CFastPlane is only held by m_cardTextures and then passed to the button listener,
    // then clearing this map would release the shared_ptr and potentially delete the surface.
    // Ensure button has its own strong reference or is passed a raw ptr if we manage its lifetime.
    // For now, let's keep it clear to release memory from old page.
    m_cardTextures.clear();
    m_fullCardPreviewPlane = smart_ptr<CFastPlane>(); // FIX: Clear full card preview with explicit smart_ptr()
}

void CSceneCardList::LoadCardTexturesForCurrentPage() {
    // This function will now update the plane for the CGUUIButton associated with each card
    ClearCardTextures(); // Clear previously loaded textures and full preview

    int startIndex = (m_nCurrentPage - 1) * CARDS_PER_PAGE;
    int endIndex = startIndex + CARDS_PER_PAGE;
    if ((size_t)endIndex > m_ownedCards.size()) {
        endIndex = m_ownedCards.size();
    }

    char filepath[256];
    for (int i = startIndex; i < endIndex; ++i) {
        CardInfo& card = m_ownedCards[i]; // Use reference to modify button

        if (card.m_cardButton.get()) { // Check if the button exists for this card
            smart_ptr<CFastPlane> cardPlane = smart_ptr<CFastPlane>(new CFastPlane());
            sprintf(filepath, "data/mini/%s.bmp", card.bmpName.c_str());

            if (cardPlane->Load(filepath) == 0) {
                m_cardTextures[card.id] = cardPlane; // Keep a reference in the map

                // Set the button's plane using the newly loaded texture
                smart_ptr<CGUIButtonEventListener> cardBtnListener = card.m_cardButton->GetEvent();
                CGUINormalButtonListener* pBtn = static_cast<CGUINormalButtonListener*>(cardBtnListener.get());
                pBtn->SetPlane(smart_ptr<ISurface>(cardPlane.get(), false)); // Button listener doesn't own surface
            } else {
                // If loading fails, clear the button's plane to avoid drawing a stale image
                smart_ptr<CGUIButtonEventListener> cardBtnListener = card.m_cardButton->GetEvent();
                CGUINormalButtonListener* pBtn = static_cast<CGUINormalButtonListener*>(cardBtnListener.get());
                pBtn->SetPlane(smart_ptr<ISurface>()); // FIX: Set plane to a null smart_ptr
                char debugStr[256];
                sprintf(debugStr, "Warning: Failed to load card texture (for button update): %s\n", filepath);
                OutputDebugStringA(debugStr);
            }
        }
    }
}

// New helper to reset animation counters for cards on the current page
void CSceneCardList::ResetCardAnimations(bool forNewPage) {
    int startIndex = (m_nCurrentPage - 1) * CARDS_PER_PAGE;
    int endIndex = startIndex + CARDS_PER_PAGE;
    if ((size_t)endIndex > m_ownedCards.size()) {
        endIndex = m_ownedCards.size();
    }

    for (int i = startIndex; i < endIndex; ++i) {
        CardInfo& card = m_ownedCards[i]; // Access by reference to modify

        if (forNewPage) {
            // For entering a new page, set counters to start from 0/negative
            card.m_nScaleAnimation.Set(0, 100, ORIGINAL_CARD_ANIM_SCALE_SPEED);
            card.m_nYOffsetAnimation.Set(-(CARD_HEIGHT / 8), 0, ORIGINAL_CARD_ANIM_SPEED); // Adjusted Y-offset
            card.m_bAnimationStarted = false; // Reset to false, will be started by UpdateCardAnimations
            card.m_bAnimationCompleted = false;
        } else {
            // For exiting current page, set counters to animate in reverse (100 to 0)
            card.m_nScaleAnimation.Set(100, 0, ORIGINAL_CARD_ANIM_SCALE_SPEED);
            card.m_nYOffsetAnimation.Set(0, -(CARD_HEIGHT / 8), ORIGINAL_CARD_ANIM_SPEED); // Animate up to offset
            card.m_bAnimationStarted = true; // Force start for exit
            card.m_bAnimationCompleted = false; // Reset for exit
        }
    }
    // RESET the scene frame counter when page changes / animations are reset
    m_currentSceneFrameCount = 0;
}

// ChangePage now only signals a page change, actual change happens in UpdateCardAnimations
void CSceneCardList::ChangePage(bool forward) {
    // This function's logic is now primarily handled by OnMove setting m_sceneAnimState
    // and UpdateCardAnimations processing the state.
    // This function will primarily be used to signal the intent to change page,
    // which then triggers the SAS_EXITING_GRID state.
}

// Helper functions for diagonal step calculation
int CSceneCardList::GetDiagonalStep_FromTopRight(int col, int row) {
    // Top-Right (col=CARD_COLUMNS-1, row=0) has lowest step (0)
    // Values increase as you move towards bottom-left.
    return (CARD_COLUMNS - 1 - col) + row;
}

int CSceneCardList::GetDiagonalStep_FromTopLeft(int col, int row) {
    // Top-Left (col=0, row=0) has lowest step (0)
    // Values increase as you move towards bottom-right.
    return col + row;
}

int CSceneCardList::GetDiagonalStep_FromBottomLeft(int col, int row) {
    // Bottom-Left (col=0, row=CARD_ROWS-1) has lowest step (0)
    // Values increase as you move towards top-right.
    return col + (CARD_ROWS - 1 - row);
}

void CSceneCardList::UpdateCardAnimations() {
    int startIndex = (m_nCurrentPage - 1) * CARDS_PER_PAGE;
    int endIndex = startIndex + CARDS_PER_PAGE;
    if ((size_t)endIndex > m_ownedCards.size()) {
        endIndex = m_ownedCards.size();
    }

    bool overallAnimationComplete = true; // Overall check for all cards on the page
    int currentFrameCount = m_currentSceneFrameCount;

    if (m_sceneAnimState == SAS_INITIAL_LOAD || m_sceneAnimState == SAS_ENTERING_GRID) {
        for (int col = 0; col < CARD_COLUMNS; col++) {
            for (int row = 0; row < CARD_ROWS; row++) {
                int collectionIndex = startIndex + (col * CARD_ROWS) + row;

                if (collectionIndex >= 0 && (size_t)collectionIndex < m_ownedCards.size()) {
                    CardInfo& currentCard = m_ownedCards[collectionIndex];

                    if (!currentCard.m_bAnimationCompleted) {
                        int diagonalStep;
                        if (m_sceneAnimState == SAS_INITIAL_LOAD || m_bForwardPageChange) {
                            // Initial load and entering from Right-Button (forward) -> Top-Right to Bottom-Left
                            diagonalStep = GetDiagonalStep_FromTopRight(col, row);
                        } else { // SAS_ENTERING_GRID and !m_bForwardPageChange (back button) -> Top-Left to Bottom-Right
                            diagonalStep = GetDiagonalStep_FromTopLeft(col, row);
                        }

                        int desiredStartFrame = diagonalStep * DIAGONAL_STAGGER_DELAY_PER_STEP;

                        if (currentFrameCount >= desiredStartFrame) {
                            currentCard.m_bAnimationStarted = true;
                        }

                        if (currentCard.m_bAnimationStarted) {
                            if (!currentCard.m_nScaleAnimation.IsEnd()) {
                                currentCard.m_nScaleAnimation.Inc(); // Scale up
                                currentCard.m_nYOffsetAnimation.Inc(); // Drop down
                                overallAnimationComplete = false;
                            } else {
                                currentCard.m_bAnimationCompleted = true;
                            }
                        } else {
                            overallAnimationComplete = false;
                        }
                    }
                }
            }
        }

        if (overallAnimationComplete) {
            m_sceneAnimState = SAS_IDLE;
        }

    } else if (m_sceneAnimState == SAS_EXITING_GRID) {
        bool allAnimationsCompleted = true;

        for (int col = 0; col < CARD_COLUMNS; col++) {
            for (int row = 0; row < CARD_ROWS; row++) {
                int collectionIndex = startIndex + (col * CARD_ROWS) + row;

                if (collectionIndex >= 0 && (size_t)collectionIndex < m_ownedCards.size()) {
                    CardInfo& card = m_ownedCards[collectionIndex];

                    if (!card.m_bAnimationCompleted) {
                        int diagonalStep;
                        if (m_bForwardPageChange) {
                            // Exiting for next page (right button) -> wave out from Top-Right to Bottom-Left
                            diagonalStep = GetDiagonalStep_FromTopRight(col, row);
                        } else {
                            // Exiting for previous page (left button) -> wave out from Top-Left to Bottom-Right
                            diagonalStep = GetDiagonalStep_FromTopLeft(col, row);
                        }

                        int desiredStartExitFrame = diagonalStep * EXIT_DIAGONAL_STAGGER_DELAY_PER_STEP;

                        if (currentFrameCount >= desiredStartExitFrame) {
                            card.m_bAnimationStarted = true;
                        }

                        if (card.m_bAnimationStarted) {
                            if (!card.m_nScaleAnimation.IsBegin()) { // Check if it's scaled to 0
                                card.m_nScaleAnimation.Dec(); // Scale down
                                card.m_nYOffsetAnimation.Dec(); // Move up (to initial offset position)
                                allAnimationsCompleted = false;
                            } else {
                                card.m_bAnimationCompleted = true;
                            }
                        } else {
                            allAnimationsCompleted = false;
                        }
                    }
                }
            }
        }

        if (allAnimationsCompleted) {
            if (m_bPageChangeRequested) {
                if (m_bForwardPageChange) {
                    m_nCurrentPage++;
                } else {
                    m_nCurrentPage--;
                }
                m_bPageChangeRequested = false;
                LoadCardTexturesForCurrentPage();
                ResetCardAnimations(true);       // Reset for new page entry
                m_sceneAnimState = SAS_ENTERING_GRID;
            }
        }
    }
}

void CSceneCardList::DrawCardGrid(const smart_ptr<ISurface>& lp) {
    const int GRID_START_X = 230;
    const int GRID_START_Y = 108;
    const int CARD_SPACING_X = 5;
    const int CARD_SPACING_Y = 15;

    int startIndex = (m_nCurrentPage - 1) * CARDS_PER_PAGE;

    // Loop through each card position from right to left, top to bottom (drawing order)
    // This ensures cards drawn on the right are on top of cards on the left, etc.
    for (int col = CARD_COLUMNS - 1; col >= 0; col--) { // Changed to iterate right to left
        for (int row = 0; row < CARD_ROWS; row++) {
            // Calculate the index in the collection (matches the logical layout: col by col, then row by row)
            int collectionIndex = startIndex + (col * CARD_ROWS) + row;

            // Only draw if within bounds and its animation has started
            if (collectionIndex >= 0 && (size_t)collectionIndex < m_ownedCards.size()) {
                CardInfo& card = m_ownedCards[collectionIndex]; // Use reference to access animation states

                // Only draw cards that are currently animating in/out or are idle (fully scaled)
                if (card.m_bAnimationStarted || m_sceneAnimState == SAS_IDLE) {
                    // Pre-calculate card-specific animation values
                    int currentYOffset = (int)card.m_nYOffsetAnimation; // FIX: Revert to (int) cast
                    int scalePercent = (int)card.m_nScaleAnimation;     // FIX: Revert to (int) cast
                    if (scalePercent < 0) scalePercent = 0; // Ensure no negative scale
                    if (scalePercent > 100) scalePercent = 100; // Ensure no over-scale

                    // Do not draw if scale is 0 (fully scaled out) AND we are in the exiting state.
                    // This prevents flickering a 0-scale card briefly during entry or idle.
                    if (scalePercent == 0 && m_sceneAnimState == SAS_EXITING_GRID) {
                        continue;
                    }

                    int scaledWidth = (CARD_WIDTH * scalePercent) / 100;
                    if (scaledWidth < 1) scaledWidth = 1; // Ensure width is at least 1 pixel
                    int scaledHeight = (CARD_HEIGHT * scalePercent) / 100;
                    if (scaledHeight < 1) scaledHeight = 1; // Ensure height is at least 1 pixel

                    int xOffset = (CARD_WIDTH - scaledWidth) / 2; // Keep centered horizontally
                    int yOffset = (CARD_HEIGHT - scaledHeight) / 2; // Keep centered vertically

                    // Calculate position on screen
                    int x = GRID_START_X + (col * (CARD_WIDTH + CARD_SPACING_X));
                    int y = GRID_START_Y + (row * (CARD_HEIGHT + CARD_SPACING_Y)) + currentYOffset;

                    if (card.m_cardButton.get()) {
                        // Set button position and size before drawing
                        card.m_cardButton->SetXY(x + xOffset, y + yOffset);
                        // Also set bounds for scaled button
                        RECT buttonBounds = {0, 0, scaledWidth, scaledHeight};
                        card.m_cardButton->SetBounds(buttonBounds);

                        // Use special scale draw operation
						card.m_cardButton->SetScaleSize(scaledWidth, CARD_HEIGHT);
                        card.m_cardButton->OnSimpleScaleDraw(lp.get());

                        // Draw hover border if the button is hovered AND fully scaled in
                        if (card.m_cardButton->IsIn() && scalePercent == 100 && m_cardHoverBorder) {
                            // FIX: Use BltFast with pDstSize and pSrcRect for scaling
                            SIZE dstSize = { scaledWidth, scaledHeight };
                            RECT srcRect = {
                                0, 0,
                                m_cardHoverBorder->GetConstSurfaceInfo()->GetSize().cx, // FIX: Use GetConstSurfaceInfo()->GetSize().cx
                                m_cardHoverBorder->GetConstSurfaceInfo()->GetSize().cy  // FIX: Use GetConstSurfaceInfo()->GetSize().cy
                            };
                            lp->BltNatural(m_cardHoverBorder.get(), x, y, &dstSize, &srcRect, NULL, 0);

							// Card TextBox
							// TODO: this needs some font tweaks and linespacing but matches more or less
							const DialogEntry* cardTypeDialog = m_bin->GetDialog(card.cardData->properties.GetMonsterTypeTextId());
							std::string formattedText = 
								"<SIZE=-1><BOLD>" + std::string(card.cardData->name.name) + "</BOLD></SIZE>" +
								"<HR>" +
								"<COLOR=#4A2C00><SIZE=0>[" + cardTypeDialog->text + "]</SIZE></COLOR>" +
								"<HR>" +
								"<COLOR=#4A2C00><SIZE=0>" + std::string(card.cardData->description) + "</SIZE></COLOR>"
								;

							MonsterType ctype = card.cardData->properties.GetMonsterType();
							if (ctype == TYPE_SPELLCARD || ctype == TYPE_TRAPCARD)
							{
								m_cardTextBoxFooter->SetText(""); // its not drawn but reset just to be sure
							}
							else
							{
								formattedText += "<HR><HR>"; // padding for PAP separator line
								WORD cardATK = card.cardData->properties.GetAttackValue();
								WORD cardDEF = card.cardData->properties.GetDefenseValue(); 
								char buffer[256];
								// %3d inserts a number padded with spaces to a width of 3 (fixing your alignment/spacing issue automatically).
								// TODO: this needs some font tweaks and linespacing but matches more or less
								sprintf(buffer, "<SIZE=0><RIGHT><COLOR=#4A2C00>ATK/%3d DEF/%3d</COLOR></RIGHT></SIZE>", cardATK, cardDEF);
								m_cardTextBoxFooter->SetText(buffer);
							}

							m_cardTextBox->SetText(formattedText); // apply text
                        }

                        // Draw "NEW" indicator only if card is new and fully scaled in
                        if (card.isNew && scalePercent == 100) {
                            CPlane newIndicator = m_vPlaneLoader.GetPlane(10);
                            if (newIndicator) {
								// TODO: this is an animation. disappears onClick()
                                lp->BltFast(newIndicator.get(), x + 5, y + 26);
                            }
                        }
                    }
                }
            }
        }
    }
}

void CSceneCardList::DrawCardPreview(const smart_ptr<ISurface>& lp) {
    // Preview area starts at position from list_detail.txt
    const int PREVIEW_X = 11;
    const int PREVIEW_Y = 88;

    // Draw background for preview area
    //lp->BltFast(&m_cardPreviewImage, PREVIEW_X, PREVIEW_Y);

    if (m_nPreviewCardId != 0 && m_fullCardPreviewPlane.get()) {
        // Draw the full-sized card image
        // We need to scale this to fit the preview area (200x290)
        // Original card is 200x290 so it should fit directly.
        // For now, assume it fits perfectly.
        lp->BltFast(m_fullCardPreviewPlane.get(), PREVIEW_X, PREVIEW_Y);

        // Draw detail line
        CPlane detailLine = m_vDetailPlaneLoader.GetPlane(3); // pap_line.bmp
        if (detailLine) {
            lp->BltFast(detailLine.get(), detailLine->GetPosX(), detailLine->GetPosY());
        }
	} else
	{
		lp->BltFast(m_fullCardPreviewPlaneUra.get(), PREVIEW_X, PREVIEW_Y);
	}
}

void CSceneCardList::DrawPagination(const smart_ptr<ISurface>& lp) {
    // Load page number font
    CPlane pageFont = m_vPlaneLoader.GetPlane(3); // page_font.bmp

    if (!pageFont) return;

    int pOffsetX = 8;
    int pOffsetY = 9;

    // Draw current page number (positions from list_scene.txt)
    char pageNum[8];
    sprintf(pageNum, "%d", m_nCurrentPage);
    int numX = 480;  // From list_scene.txt
    int numY = 561;

    for (char* p = pageNum; *p; p++) {
        int digit = *p - '0';
        RECT srcSize = { digit * 15, 0, digit * 15 + 15, 19 }; // Font character width is 15px, height 19px
        lp->BltNatural(pageFont.get(), numX - pOffsetX, numY - pOffsetY, 0, &srcSize);
        numX += 21; // Advance X for next digit
    }

    // Draw total pages
    sprintf(pageNum, "%d", m_nTotalPages);
    numX = 527;  // From list_scene.txt

    for (char* p = pageNum; *p; p++) {
        int digit = *p - '0';
        RECT srcSize = { digit * 15, 0, digit * 15 + 15, 19 }; // Font character width is 15px, height 19px
        lp->BltNatural(pageFont.get(), numX - pOffsetX, numY - pOffsetY, 0, &srcSize);
        numX += 21; // Advance X for next digit
    }
}

// TODO: This needs further code. Switch all few seconds from Percentage% to OwnedCards/AllCards label
void CSceneCardList::DrawCollectionRate(const smart_ptr<ISurface>& lp) {
    // Get font and symbol planes
    CPlane rateFont = m_vPlaneLoader.GetPlane(11);    // get_font.bmp
    CPlane rateSymbol = m_vPlaneLoader.GetPlane(12);  // get_symbol.bmp

    if (!rateFont || !rateSymbol) return;

    float rate = 0.0f;
    if (m_nTotalPages > 0) {
        // This calculates percentage of grid filled, not collection rate of all cards.
        // It should be m_ownedCards.size() / total_game_cards * 100.0f if 'total_game_cards' exists.
        rate = (float)m_ownedCards.size() / (m_nTotalPages * CARDS_PER_PAGE) * 100.0f;
    }

    char rateStr[16];
    sprintf(rateStr, "%.1f", rate);

    // Draw at position from list_scene.txt (761,080)
    int numX = 670;
    int numY = 80;

    // Draw digits
    for (char* p = rateStr; *p; p++) {
        if (*p == '.') {
            RECT srcSize = { 11, 0, 17, 18 };
            lp->BltNatural(rateSymbol.get(), numX, numY + 12, 0, &srcSize);
            numX += 6;
            continue;
        }

        int digit = *p - '0';
        RECT srcSize = { digit * 12, 0, digit * 12 + 12, 15 };
        lp->BltNatural(rateFont.get(), numX, numY, 0, &srcSize);
        numX += 10;
    }

    // Draw % symbol
    RECT rectPercent = { 15, 0, 36, 18 };
    lp->BltNatural(rateSymbol.get(), numX + 2, numY - 1, 0, &rectPercent);
}