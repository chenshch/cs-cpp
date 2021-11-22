﻿/****************************************************************************
 Copyright (c) 2015 Zhangpanyi

 Created by Zhangpanyi on 2015

 zhangpanyi@live.com
 ****************************************************************************/
#pragma once

#include <deque>
#include <vector>
#include <functional>
#include "NonCopyable.h"
#include "BlockAllocator.h"

#define NOTEXIST		0
#define IN_OPENLIST		1
#define IN_CLOSELIST	2

 /**
  * 位置
  */
struct APoint
{
	unsigned short row;
	unsigned short col;

	APoint() : row(0), col(0) {}
	APoint(unsigned short row, unsigned short col) : row(row), col(col) {}

	bool operator== (const APoint& that) const
	{
		return row == that.row && col == that.col;
	}

	const APoint& operator() (unsigned short row, unsigned short col)
	{
		this->row = row;
		this->col = col;
		return *this;
	}
};

/**
 * 查询函数
 */
typedef std::function<bool(const APoint&)> QueryCallBack;

/**
 * A*算法参数定义
 */
struct AStarDef
{
	bool			allow_corner;
	unsigned short	row;
	unsigned short	col;
	APoint			start_point;
	APoint			end_point;
	QueryCallBack	can_reach;

	AStarDef() : row(0), col(0), can_reach(nullptr), allow_corner(false) {}
};

class AStar : public NonCopyable
{
public:
	/**
	 * 格子结点
	 * 记录g值、h值、和父节点信息
	 * 使用小对象分配器分配内存
	 */
	struct Node
	{
		unsigned short	g;
		unsigned short	h;
		APoint			pos;
		int				state;
		Node* parent;

		int f() const
		{
			return g + h;
		}

		Node(const APoint& pos) : g(0), h(0), pos(pos), parent(nullptr), state(NOTEXIST) {}

		void* operator new(std::size_t size)
		{
			void* ptr = SOA::getInstance()->Allocate(size);
			return ptr;
		}

		void operator delete(void* p) throw()
		{
			if (p) SOA::getInstance()->Free(p, sizeof(Node));
		}
	};

public:
	AStar();
	~AStar();

public:
	/**
	 * 执行A*搜索
	 * @ 参数 def A*算法参数定义
	 * @ 返回搜索路径
	 */
	std::deque<APoint> Search(const AStarDef& def);

private:
	/**
	 * 清理
	 */
	void Clear();

	/**
	 * 初始化
	 * @ 参数 def A*算法参数定义
	 */
	void Init(const AStarDef& def);

	/**
	 * 检测 A*算法参数定义是否有效
	 * @ 参数 def A*算法参数定义
	 */
	bool ValidAStarDef(const AStarDef& def);

private:
	/**
	 * 格子是否存在于开启列表
	 * @ 参数 Point 位置
	 * @ 存在返回格子结点的指针，不存在返回nullptr
	 */
	Node* IsExistInOpenList(const APoint& point);

	/**
	 * 格子是否存在于关闭列表
	 * @ 参数 Point 位置
	 * @ 存在返回true，不存在返回false
	 */
	bool IsExistInCloseList(const APoint& point);

	/**
	 * 查询格子是否可通行
	 * @ 参数 target_point 目标点
	 * @ 成功返回true，失败返回false
	 */
	bool IsCanReach(const APoint& target_point);

	/**
	 * 查询格子是否可到达
	 * @ 参数 current_point 当前点, target_point 目标点, allow_corner 是否允许斜走
	 * @ 成功返回true，失败返回false
	 */
	bool IsCanReached(const APoint& current_point, const APoint& target_point, bool allow_corner);

	/**
	 * 搜索可通行的格子
	 * @ 参数 current_point 当前点, allow_corner 是否允许斜走, surround_point 存放搜索结果的数组
	 */
	void SearchCanReached(const APoint& current_point, bool allow_corner, std::vector<APoint>& surround_point);

	/**
	 * 计算G值
	 * @ 参数 parent 父节点, current_point 当前点
	 */
	unsigned int CalculG(Node* parent, const APoint& current_point);

	/**
	 * 计算H值
	 * @ 参数 current_point 当前点, end_point 终点
	 */
	unsigned int CalculH(const APoint& current_point, const APoint& end_point);

	/**
	 * 获取节点在开启列表中的索引值
	 * @ 失败返回-1
	 */
	int GetIndex(Node* node);

	/**
	 * 开启列表上滤(二叉堆上滤)
	 * @ 参数 hole 上滤位置
	 */
	void PercolateUp(int hole);

private:
	/**
	 * 当节点存在于开启列表中的处理函数
	 */
	void FoundNode(Node* current_point, Node* new_point);

	/**
	 * 当节点不存在于开启列表中的处理函数
	 */
	void NotFoundNode(Node* current_point, Node* new_point, const APoint& end);

private:
	unsigned short		num_row_;			// 地图行数
	unsigned short		num_col_;			// 地图列数
	unsigned int		num_map_size_;		// 节点地图大小
	QueryCallBack		query_func_;		// 查询函数
	std::vector<Node*>	open_list_;			// 开启列表
	std::vector<Node*>	maps_index_;		// 地图索引
};