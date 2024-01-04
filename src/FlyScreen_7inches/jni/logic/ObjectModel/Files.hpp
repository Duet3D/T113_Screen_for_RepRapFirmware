/*
 * Files.hpp
 *
 *  Created on: 3 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_OBJECTMODEL_FILES_HPP_
#define JNI_LOGIC_OBJECTMODEL_FILES_HPP_

#include <string>
#include <vector>

namespace OM
{
	class Folder;

	class FileSystemItem
	{
	public:
		FileSystemItem() : pParent_(nullptr) {}
		FileSystemItem(Folder* parent) : pParent_(parent) {}
		FileSystemItem(const std::string& name) : name_(name), pParent_(nullptr) {}
		FileSystemItem(const std::string& name, Folder* parent) : name_(name), pParent_(parent) {}

		const std::string& GetName() const { return name_; }
		void SetName(const std::string& name) { name_ = name; }
		std::string GetPath();
	private:
		std::string name_;
		Folder* pParent_;
	};

	class File : public FileSystemItem
	{
	public:
		File(Folder* parent) : FileSystemItem(parent), size_(0) {}
		File(const std::string& name, Folder* parent) : FileSystemItem(name, parent), size_(0) {}
		File(const std::string& name, const size_t size, Folder* parent) : FileSystemItem(name, parent), size_(size) {}

		const std::string& GetDate() const { return date_; }
		void SetDate(const std::string& date) { date_ = date; }
		size_t GetSize() const { return size_; }
		void SetSize(const size_t size) { size_ = size; }

	private:
		size_t size_;
		std::string date_;
	};


	class Folder : public FileSystemItem
	{
	public:
		Folder(Folder* parent) : FileSystemItem(parent) {}
		Folder(const std::string& name) : FileSystemItem(name) {}
		Folder(const std::string& name, Folder* parent) : FileSystemItem(name, parent) {}
		File* AddFile()
		{
			files_.push_back(File(this));
			return &files_.back();
		}

		File* AddFile(const std::string& name)
		{
			files_.push_back(File(name, this));
			return &files_.back();
		}

		File* AddFile(const File file)
		{
			files_.push_back(file);
			return &files_.back();
		}

		Folder* AddFolder()
		{
			folders_.push_back(Folder(this));
			return &folders_.back();
		}

		Folder* AddFolder(const std::string& name)
		{
			folders_.push_back(Folder(name, this));
			return &folders_.back();
		}

		Folder* AddFolder(const Folder folder)
		{
			folders_.push_back(folder);
			return &folders_.back();
		}

		File* GetFile(const std::string& name);
		Folder* GetSubFolder(const std::string& name);
		File* GetOrCreateFile(const std::string& name);
		Folder* GetOrCreateSubFolder(const std::string& name);

		void Clear()
		{
			files_.clear();
			folders_.clear();
		}

	private:
		std::vector<File> files_;
		std::vector<Folder> folders_;
	};

	void SetCurrentDir(const std::string& path);
	std::string& GetCurrentDirPath();
	Folder* GetCurrentFolder();
} /* namespace OM */



#endif /* JNI_LOGIC_OBJECTMODEL_FILES_HPP_ */
