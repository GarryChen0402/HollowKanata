#pragma once

#include "util.h"
#include "timer.h"
#include "atlas.h"
#include "vector2.h"

#include <vector>
#include <functional>
#include <iostream>

class Animation {
public:
	enum class AnchorMode
	{
		Centered,
		BottomCentered
	};

private:
	struct Frame
	{
		Rect rect_src;
		IMAGE* image = nullptr;

		Frame() = default;
		Frame(IMAGE* image, const Rect& rect_src) : image(image), rect_src(rect_src) {}

		~Frame() = default;
	};

private:
	Timer timer;
	Vector2 position;
	bool is_loop = true;
	size_t idx_frame = 0;
	std::vector<Frame> frame_list;
	std::function<void()> on_finished;
	AnchorMode anchor_mode = AnchorMode::Centered;


public:
	Animation() {
		timer.set_one_shot(false);
		timer.set_on_timeout(
			[&]()
			{
				idx_frame++;
				if (idx_frame >= frame_list.size())
				{
					idx_frame = is_loop ? 0 : frame_list.size() - 1;
					if (!is_loop && on_finished)on_finished();
				}
			}
		);
	}

	~Animation() {

	}

	void reset() {
		timer.restart();
		idx_frame = 0;
	}

	void set_anchor_mode(AnchorMode mode) {
		anchor_mode = mode;
	}

	void set_position(const Vector2& position) {
		this->position = position;
	}

	void set_loop(bool is_loop) {
		this->is_loop = is_loop;
	}

	void set_interval(float interval) {
		timer.set_wait_time(interval);
	}

	void set_on_finished(std::function<void()> on_finished) {
		this->on_finished = on_finished;
	}

	void add_frame(IMAGE* image, int num_h)
	{
		int width = image->getwidth();
		int height = image->getheight();
		int width_frame = width / num_h;

		for (int i = 0; i < num_h; i++) {
			Rect rect_src = {
				i * width_frame,
				0,
				width_frame,
				height
			};
			frame_list.emplace_back(image, rect_src);
		}
	}

	void add_frame(Atlas* atlas)
	{
		for (int i = 0; i < atlas->get_size(); i++) {
			IMAGE* image = atlas->get_image(i);
			int width = image->getwidth();
			int height = image->getheight();

			Rect rect_src = {
				0,
				0,
				width,
				height
			};
			frame_list.emplace_back(image, rect_src);
		}
	}
	
	void on_update(float delta)
	{
		timer.on_update(delta);
	}

	void on_render()
	{
		std::cout << "idx_frame = " << idx_frame <<"  " << frame_list.size() << std::endl;
		const Frame& frame = frame_list[idx_frame];
		std::cout << "frame load complete" << std::endl;
		Rect rect_dst = {
			(int)position.x - frame.rect_src.w / 2,
			(anchor_mode == AnchorMode::Centered) ? (int)position.y - frame.rect_src.h / 2 : (int)position.y - frame.rect_src.h,
			frame.rect_src.w,
			frame.rect_src.h
		};

		putimage_ex(frame.image, &rect_dst, &frame.rect_src);
	}

	void set_all_data(float interval, bool is_loop, AnchorMode anchor_mode, IMAGE* image, int num_h)
	{
		set_interval(interval);
		set_loop(is_loop);
		set_anchor_mode(anchor_mode);
		add_frame(image, num_h);
	}

	void set_all_data(float interval, bool is_loop, AnchorMode anchor_mode, IMAGE* image, int num_h, std::function<void()> on_finished)
	{
		set_all_data(interval, is_loop, anchor_mode, image, num_h);
		set_on_finished(on_finished);
	}

	int get_frame_list_size()
	{
		return frame_list.size();
	}

};