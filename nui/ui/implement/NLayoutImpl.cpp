#include "stdafx.h"
#include "../NLayout.h"

#include "../NRenderClip.h"

#include "NHorzLayoutArranger.h"
#include "NVertLayoutArranger.h"
#include "NTileLayoutArranger.h"

#undef min
#undef max

namespace nui
{
    namespace Ui
    {
        IMPLEMENT_REFLECTION(NLayout);

        NLayout::NLayout() : innerFrame_(MemToolParam)
        {
            currentLayoutType_ = LayoutNone;
            SetLayoutType(LayoutVert);

            innerFrame_->SetAutoSize(false);
            AddChild(innerFrame_);
        }

        NLayout::~NLayout()
        {
        }

        void NLayout::SetLayoutType(NLayoutType layoutType)
        {
            if(layoutType == currentLayoutType_)
                return;
            switch(layoutType)
            {
            case LayoutHorz:
                layoutArranger_ = NNew(NHorzLayoutArranger);
                break;
            case LayoutVert:
                layoutArranger_ = NNew(NVertLayoutArranger);
                break;
            case LayoutTile:
                layoutArranger_ = NNew(NTileLayoutArranger);
                break;
            default:
                layoutArranger_ = NULL;
            }
            currentLayoutType_ = layoutType;
            RelayoutChilds();
        }

        bool NLayout::AddChild(NFrameBase* child)
        {
            if(IsLayoutChild(child))
            {
                return __super::AddChild(child);
            }
            else
            {
                bool result = innerFrame_->AddChild(child);
                if(result)
                    RelayoutChilds();
                AutoSize();
                return result;
            }
        }

        bool NLayout::RemoveChild(NFrameBase* child)
        {
            if(IsLayoutChild(child))
            {
                return __super::AddChild(child);
            }
            else
                {
                bool result = innerFrame_->RemoveChild(child);
                if(result)
                    RelayoutChilds();
                AutoSize();
                return result;
            }
        }

        void NLayout::RemoveAllChilds()
        {
            innerFrame_->RemoveAllChilds();
            RelayoutChilds();
            AutoSize();
        }

        void NLayout::Draw(NRender* render, Base::NPoint& ptOffset, HRGN clipRgn)
        {
            __super::Draw(render, ptOffset, clipRgn);
        }

        Base::NSize NLayout::GetAutoSize() const
        {
            Base::NSize size = innerFrame_->GetAutoSize();

            size.Width = std::max(size.Width, innerFrame_->GetRect().Width());
            size.Height = std::max(size.Height, innerFrame_->GetRect().Height());
            return size;
        }

        void NLayout::RelayoutChilds()
        {
            NLayoutArrangerParam param;
            param.data_ = 0;
            param.frameSize_ = frameRect_.GetSize();
            innerFrame_->EnumChilds(MakeDelegate(this, &NLayout::OnEnumChild), reinterpret_cast<LPARAM>(&param));

            Base::NRect rcLayout = GetRect();
            if(rcLayout.Width() >= param.maxSize_.Width && rcLayout.Height() >= param.maxSize_.Height)
            {
                // both hide
                if(horzScroll_)
                    horzScroll_->SetVisible(false);
                if(vertScroll_)
                    vertScroll_->SetVisible(false);
            }
            else if(param.maxSize_.Width + GetVertScroll()->GetRect().Width() <= frameRect_.Width() && param.maxSize_.Height > frameRect_.Height())
            {
                // horz hide, vert show
                if(horzScroll_)
                    horzScroll_->SetVisible(false);
                NScroll* vertScroll = GetVertScroll();
                vertScroll->SetVisible(true);
                vertScroll->SetScrollPage(GetRect().Height());
                vertScroll->SetScrollRange(param.maxSize_.Height - GetRect().Height());
            }
            else if(param.maxSize_.Height + GetHorzScroll()->GetRect().Height() <= frameRect_.Height() && param.maxSize_.Width > frameRect_.Width())
            {
                // vert hide, horz show
                if(vertScroll_)
                    vertScroll_->SetVisible(false);
                NScroll* horzScroll = GetHorzScroll();
                horzScroll->SetMargin(0, 0, 0, 0);
                horzScroll->SetScrollPage(GetRect().Width());
                horzScroll->SetScrollRange(param.maxSize_.Width - GetRect().Width());
            }
            else
            {
                NScroll* horzScroll = GetHorzScroll();
                NScroll* vertScroll = GetVertScroll();

                horzScroll->SetVisible(true);
                horzScroll->SetMargin(0, 0, vertScroll->GetRect().Width(), 0);
                int horzPage = GetRect().Width() - vertScroll->GetRect().Width();
                horzScroll->SetScrollPage(horzPage);
                horzScroll->SetScrollRange(param.maxSize_.Width - horzPage);

                vertScroll->SetVisible(true);
                int vertPage = GetRect().Height() - horzScroll->GetRect().Height();
                vertScroll->SetScrollPage(vertPage);
                vertScroll->SetScrollRange(param.maxSize_.Height - vertPage);
            }

            innerFrame_->SetSize(param.maxSize_.Width, param.maxSize_.Height);
        }

        bool NLayout::OnEnumChild(NFrameBase* child, LPARAM lParam)
        {
            NLayoutArrangerParam* param = reinterpret_cast<NLayoutArrangerParam*>(lParam);

            layoutArranger_->RelayoutChild(child, *param);
            return true;
        }

        void NLayout::OnHorzScrolled(NScroll*, int scrollPos)
        {
            innerFrame_->SetPos(-scrollPos, innerFrame_->GetRect().Top);
        }

        void NLayout::OnVertScrolled(NScroll*, int scrollPos)
        {
            innerFrame_->SetPos(innerFrame_->GetRect().Left, -scrollPos);
        }

        NScroll* NLayout::GetHorzScroll()
        {
            if(horzScroll_)
                return horzScroll_;
            Base::NInstPtr<NScroll> scroll(MemToolParam);
            horzScroll_ = scroll;
            horzScroll_->SetScrollType(true);
            AddChild(horzScroll_);
            horzScroll_->SetLayout(NFrame::LayoutHFill | NFrame::LayoutBottom);
            horzScroll_->SetScrollCallback(MakeDelegate(this, &NLayout::OnHorzScrolled));
            return horzScroll_;
        }

        NScroll* NLayout::GetVertScroll()
        {
            if(vertScroll_)
                return vertScroll_;
            Base::NInstPtr<NScroll> scroll(MemToolParam);
            vertScroll_ = scroll;
            vertScroll_->SetScrollType(false);
            AddChild(vertScroll_);
            vertScroll_->SetLayout(NFrame::LayoutVFill | NFrame::LayoutRight);
            vertScroll_->SetScrollCallback(MakeDelegate(this, &NLayout::OnVertScrolled));
            return vertScroll_;
        }

        bool NLayout::IsLayoutChild(NFrameBase* child) const
        {
            return child == (NFrameBase*)horzScroll_
                || child == (NFrameBase*)vertScroll_
                || child == (NFrameBase*)innerFrame_;
        }
    }
}
